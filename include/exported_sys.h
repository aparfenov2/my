/*
* ����������� ���������� ����� ���������� ������ ��� ������ � �������� ����� � ���� ��������
*/

#ifndef _MESSAGES_SYS_H
#define _MESSAGES_SYS_H


#include "types.h"
#include "app_events.h"


namespace myvi {

// ��������������� ���������� ���������� �����

class host_interface2_t {
public:

	// ����� �� ������ �� ������ ������ �� ������
	virtual void read_model_data_response(char * path, char * string_value, u32 code) = 0;
	virtual void read_model_data_response(char * path, s32 int_value, u32 code ) = 0;
	virtual void read_model_data_response(char * path, double float_value, u32 code ) = 0;

	// ����� �� ������ ������ � ������
	virtual void write_model_data_ack(u32 code) = 0;

	// ����������� ������ ����� (����� ������� �� ����������)
	// file_id - ����������
	// offset - �������� �� ������ �����, ����
	// crc - �����������, crc
	// first - ������� ������ ������ �� �����
	// data - ��������� �� ������ ����������� ������
	// len - ����� �������
	virtual void download_response(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) = 0;
	// ����������� ������ �� ������� ������
	// file_id - ����������
	// cur_len - ������� ����� �����, ����
	// max_len - ����������� ���������� ����� �����, ����
	// crc - ���� != 0
	virtual void file_info_response(u32 file_id, u32 cur_len, u32 max_len, u32 crc) = 0;
	// ������������ ��� ������ ��������� �������� ��������
	virtual void error(u32 code) = 0;
};


class exported_interface2_t {
public:

	// ������ �� ������ ������ �� ������
	virtual void read_model_data(char * path) = 0;

	// ������ �� ������ ������ � ������
	virtual void write_model_data(char * path, char * string_value) = 0;
	virtual void write_model_data(char * path, s32 int_value) = 0;
	virtual void write_model_data(char * path,  double float_value) = 0;

	// ������� ����������
	virtual void key_event(key_t::key_t key) = 0;
	// �������� ������ �����
	// file_id - ����������
	// offset - �������� �� ������ �����, ����
	// crc - �����������, crc
	// first - ������� ������ ������ �� �����
	// data - ��������� �� ������ ������������ ������
	// len - ����� �������
	virtual void upload_file(u32 file_id, u32 offset, u32 crc, bool first, u8* data, u32 len) = 0;
	// ��������� ������ �����
	// file_id - ����������
	// offset - �������� �� ������ �����, ����
	// length - �����, ����
	virtual void download_file(u32 file_id, u32 offset, u32 length) = 0;
	// �������� ���������� � ������� ������
	// file_id - ����������
	// cur_len - ������� ����� �����, ����
	// max_len - ����������� ���������� ����� �����, ����
	// crc - ���� != 0
	virtual void update_file_info(u32 file_id, u32 cur_len, u32 max_len, u32 crc) = 0;
	// ��������� ���������� � �����
	// file_id - ����������
	virtual void read_file_info(u32 file_id) = 0;
};

} // ns

#endif
