<#import "../inc/link_sys_utils.ftl" as common>
/*
 * Описание реализации интерфейса связи системного уровня
 * ЭТО АВТОКОПИЯ !!! *
 *
 *  Created on: 12.03.2013
 *      Author: gordeev
 */

#ifndef SERIALIZER_H_${proto.proto.options.@namespace}
#define SERIALIZER_H_${proto.proto.options.@namespace}

#include "link.h"
#include "exported_sys_${proto.proto.options.@namespace}.h"
#include "link_sys_impl_common.h"
#include "link_sys_impl_gen_${proto.proto.options.@namespace}.h"

namespace ${proto.proto.options.@namespace} {

<#macro emit_ser_class class_name intf_name impl_name>
class ${class_name} :
	public link::packet_sender_t,
	public link::_internal_frame_receiver_t {

public:
<#list proto.proto.interfaces.interface as intf>
	<#if intf.@name?substring(0,intf_name?length) == intf_name>
	${intf.@name}_serializer_t ${intf.@name}_serializer;
	</#if>
</#list>
	link::framer_t framer;
public:

	void add_implementation(${impl_name} *impl) {
<#list proto.proto.interfaces.interface as intf>
	<#if intf.@name?substring(0,intf_name?length) == intf_name>
		<#local eintf = common.find_interface(intf.@opposite_interface)>
		<#assign exported_var_name = eintf.@name?substring(0,eintf.@name?length - 2)>
		${eintf.@name} *${exported_var_name} = dynamic_cast<${eintf.@name}*>(impl);
		if (${exported_var_name}) {
			${intf.@name}_serializer.set_implementation(${exported_var_name});
		}
	</#if>
</#list>
	}

	void init(link::serial_interface_t *asintf ) {
		framer.init(asintf, this);
<#list proto.proto.interfaces.interface as intf>
	<#if intf.@name?substring(0,intf_name?length) == intf_name>
		${intf.@name}_serializer.init(this);
	</#if>
</#list>
	}
	

	void send_packet(void *packet, u32 sizeof_packet) OVERRIDE;

<#list proto.proto.interfaces.interface as intf>
	<#if intf.@name?substring(0,intf_name?length) == intf_name>
		<#assign host_var_name = intf.@name?substring(0,intf.@name?length - 2)>
	${intf.@name} * get_${host_var_name}()  {
		return & ${intf.@name}_serializer;
	}
	</#if>
</#list>

// ------------------- public serial_data_receiver_t --------------------
	// прием фрейма данных
	void receive_frame(u8 *data, u32 len) OVERRIDE;
	// called from receive_frame
	void _receive_frame(void *pkt, u32 sizeof_pkt) {
<#list proto.proto.interfaces.interface as intf>
	<#if intf.@name?substring(0,intf_name?length) == intf_name>
		${intf.@name}_serializer.receive_packet(pkt, sizeof_pkt);
	</#if>
</#list>
	}

}; // class
</#macro>

<@emit_ser_class "serializer_t" "host" "exported_system_interface_t" />

<@emit_ser_class "host_serializer_t" "exported" "host_system_interface_t" />

} // ns myvi
#endif /* SERIALIZER_H_ */
