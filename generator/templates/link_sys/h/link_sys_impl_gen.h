<#import "../inc/link_sys_utils.ftl" as common>

<#macro emit_classdef intf>
class ${intf.@name}_serializer_t : public ${intf.@name} {
public:
	${intf.@opposite_interface} *exported;
	packet_sender_t *sender;
public:
	${intf.@name}_serializer_t() {
		exported = 0;
		sender = 0;
	}

	void init(packet_sender_t *_sender) {
		sender = _sender;
	}

	void set_implementation(${intf.@opposite_interface} *_exported) {
		exported = _exported;
	}

	<#local opposite_intf = common.find_interface(intf.@opposite_interface)>
	// expect ${opposite_intf.@message} message instance
	void receive_packet(void *packet, u32 sizeof_packet);

	<#list intf.method as mtd>
	virtual void ${mtd.@name} (${common.make_arg_str(intf, mtd)}) OVERRIDE;
	</#list>	
};



</#macro>

/*
* Определение интерфейса связи системного уровня для прямой и обратной связи с этой системой
* АВТОМАТИЧЕСКИ СГЕНЕРИРОВАННЫЙ ФАЙЛ !
*/

#ifndef _LINK_SYS_IMPL_GEN
#define _LINK_SYS_IMPL_GEN

#include "exported_sys.h"
#include "link_sys_impl_common.h"

namespace link {

<#list proto.proto.interfaces.interface as intf>
	<@emit_classdef intf />
</#list>

}
#endif