<#import "../inc/link_sys_utils.ftl" as common>

<#function has_opt_args mtd>
	<#list mtd.opt_arg as opt>
		<#return true>
	</#list>
	<#return false>
</#function>

<#macro emit_param_transfer fld mtd>
	<#local arg_cnt = 0>
	<@common.enum_field_args fld mtd;arg_name,type,ctype >
		<#if arg_cnt gt 0>
			<#lt>,
		</#if>
		<#nested ctype,arg_name>
		<#if type == "string" || type == "bytes">
			<#lt>.c_str()<#rt>
		</#if>
		<#local arg_cnt = arg_cnt + 1>
	</@common.enum_field_args>
</#macro>

<#macro emit_classimpl intf>
	<#local intf_msg = common.find_msgdef(intf.@message)>
// ======================= методы интерфейса ${intf.@name} ======================
	<#list intf.method as mtd>
		<#local fld = common.match_field(intf_msg, mtd.@name)>
void ${intf.@name}_serializer_t::${mtd.@name} (${common.make_arg_str(intf, mtd)}) {
	proto::${intf.@message} intf;
	<#if common.is_base_type(fld.@type)>
	intf.set_${fld.@name}(${fld.@name});
	<#else>
	proto::${fld.@type} *pkt = intf.mutable_${fld.@name}();
		<@common.enum_field_args fld mtd;arg_name,type,ctype >
		<#if type == "bytes">
	pkt->set_${arg_name}(${arg_name}, len);
		<#else>
	pkt->set_${arg_name}(${arg_name});
		</#if>
		</@common.enum_field_args>
	</#if>
	sender->send_packet(&intf, sizeof(intf));
}

	</#list>	

	<#local opposite_intf = common.find_interface(intf.@opposite_interface)>
	<#local opposite_intf_msg = common.find_msgdef(opposite_intf.@message)>
// expect ${opposite_intf.@message} message instance
void ${intf.@name}_serializer_t::receive_packet(void *packet, u32 sizeof_packet) {

	_MY_ASSERT(sizeof_packet == sizeof(proto::${opposite_intf.@message}), return);
	if (!this->exported) return;

	proto::${opposite_intf.@message} *ei = (proto::${opposite_intf.@message} *)packet;
	<#list opposite_intf.method as mtd>
	if (ei->has_${mtd.@name}()) {
		<#local has_opts = has_opt_args(mtd)>
		<#if has_opts>
		do {
			<#list mtd.opt_arg as opt>
			if (!ei->${mtd.@name}().has_${opt.@name}()) break;
			</#list>
		</#if>
		<#local fld = common.match_field(opposite_intf_msg, mtd.@name)>
		exported->${mtd.@name}(
		<@emit_param_transfer fld mtd; ctype, arg_name>
			<#if common.is_base_type(fld.@type)>
			(${ctype})ei->${arg_name}()<#rt>
			<#else>
			(${ctype})ei->${mtd.@name}().${arg_name}()<#rt>
			</#if>
		</@emit_param_transfer>
			${"\r\n"}		);
		<#if has_opts>
		} while(false);
		</#if>
	}
	</#list>	
		
}
	
</#macro>

/*
* Определение интерфейса связи системного уровня для прямой и обратной связи с этой системой
* АВТОМАТИЧЕСКИ СГЕНЕРИРОВАННЫЙ ФАЙЛ !
*/

#include "link_sys_impl_gen_${proto.proto.options.@namespace}.h"
#include "assert_impl.h"
#include "myvi.pb.h"

using namespace ${proto.proto.options.@namespace};

<#list proto.proto.interfaces.interface as intf>
	<@emit_classimpl intf />
</#list>
