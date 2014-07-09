#include "gtest/gtest.h"
#include "link_facade.h"
#include "exported_stub.h"
#include "basic.h"

class loopback_serial_interface_t : public link::serial_interface_t {
public:
	loopback_serial_interface_t *remote;
	link::serial_data_receiver_t *receiver;
public:
	loopback_serial_interface_t() {
		remote = 0;
		receiver = 0;
	}

	virtual void send(u8 *data, u32 len) OVERRIDE {
		ASSERT_TRUE(remote);
		ASSERT_TRUE(remote->receiver);
		remote->receiver->receive(data,len);
	}

	virtual void subscribe(link::serial_data_receiver_t *_receiver) OVERRIDE {
		receiver = _receiver;
	}

	void wire(loopback_serial_interface_t *_remote) {
		remote = _remote;
	}
};

class host_impl_t : public link::host_interface_empty_impl_t {
public:
	bool received;
public:
	host_impl_t() {
		received = false;
	}

	virtual void dme_ch_changed(u8 v) OVERRIDE {
		EXPECT_EQ(128, v);
		received = true;
	}
};

class exported_sys_impl_t : public link::exported_system_interface_t {
public:
	bool received;
public:
	exported_sys_impl_t() {
		received = false;
	}
	// запрос на чтение данных из модели
	virtual void read_model_data(char * path) OVERRIDE {
		FAIL();
	}

	// запрос на запись данных в модель
	virtual void write_model_data(char * path, char * string_value) OVERRIDE {
		FAIL();
	}

	virtual void write_model_data(char * path, s32 int_value) OVERRIDE {
		EXPECT_EQ(myvi::string_t("dme.ch"), myvi::string_t(path));
		EXPECT_EQ(127, int_value);
		received = true;
	}

	virtual void write_model_data(char * path,  double float_value) OVERRIDE {
		FAIL();
	}


	// событие клавиатуры
	virtual void key_event(myvi::key_t::key_t key) OVERRIDE {
		FAIL();
	}

	virtual void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) OVERRIDE {
		FAIL();
	}

	virtual void download_file(u32 file_id, u32 offset, u32 length) OVERRIDE {
		FAIL();
	}

	virtual void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) OVERRIDE {
		FAIL();
	}

	virtual void read_file_info(u32 file_id) OVERRIDE {
		FAIL();
	}


};


TEST(LinkTest, Loopback) {

	loopback_serial_interface_t loopback_host;
	loopback_serial_interface_t loopback_local;
	loopback_local.wire(&loopback_host);
	loopback_host.wire(&loopback_local);

// host side
	link::host_facade_t host_facade;
	host_impl_t ahost;
	host_facade.init(&ahost, &loopback_host);

// client side
	link::local_facade_t local_facade;
	exported_sys_impl_t exported_sys_impl;
	local_facade.init(&exported_sys_impl, &loopback_local);

// host->local
	host_facade.get_app_interface()->set_dme_ch(127);
// local->host
	local_facade.get_sys_interface()->read_model_data_response("dme.ch",(s32)128, 0);

	EXPECT_TRUE(ahost.received);
	EXPECT_TRUE(exported_sys_impl.received);
}
