<#function find_msgdef name>
	<#local msgdef = proto["proto/messages/message[@name='"+name+"']"]>
	<#if !msgdef?? || !msgdef?has_content>
		<#stop "cant find msgdef: "+name>
	</#if>
	<#return msgdef>
</#function>

<#function find_interface name>
	<#local intf = proto["proto/interfaces/interface[@name='"+name+"']"]>
	<#if !intf?? || !intf?has_content>
		<#stop "cant find interface: "+name>
	</#if>
	<#return intf>
</#function>

<#function match_field msg name>
	<#local fld = msg["field[@name='"+name+"']"]>
	<#if !fld?? || !fld?has_content>
		<#stop "cant find fld: "+name +" in msg:" +msg.@name>
	</#if>
	<#return fld>
</#function>

<#function is_base_type type>
	<#switch type>
		<#case "int32"><#return true>
		<#case "uint32"><#return true>
		<#case "string"><#return true>
		<#case "double"><#return true>
		<#case "bytes"><#return true>
		<#case "bool"><#return true>
	</#switch>
	<#return false>
</#function>

<#function match_ctype type>
	<#switch type>
		<#case "int32"><#return "s32">
		<#case "uint32"><#return "u32">
		<#case "string"><#return "const char *">
		<#case "double"><#return "double">
		<#case "bytes"><#return "u8 *">
		<#case "bool"><#return "bool">
	</#switch>
	<#stop "cant match ctype for type "+ type >
</#function>

<#macro enum_base_field_arg fld>
	<#if !is_base_type(fld.@type)>
		<#stop "field" + fld.@name + ": type not base: "+ flda.@type>
	</#if>
	<#local ctype = match_ctype(fld.@type)>
	<#nested fld.@name, ctype>
</#macro>

<#macro enum_field_args fld mtd>
	<#if is_base_type(fld.@type)>
		<@enum_base_field_arg fld; name,ctype>
			<#nested name,fld.@type,ctype>
		</@enum_base_field_arg>
	<#else>
		<#local fld_msg = find_msgdef(fld.@type)>
		<#list fld_msg.field as fld0>
			<#if fld0.@required?has_content && fld0.@required == "true">
				<@enum_base_field_arg fld0; name,ctype>
					<#nested name,fld0.@type,ctype>
				</@enum_base_field_arg>
			</#if>
		</#list>
		<#-- добавляем опциональные арги -->
		<#list mtd.opt_arg as arg>
			<#local flda = match_field(fld_msg arg.@name)>
			<@enum_base_field_arg flda; name,ctype>
				<#nested name,flda.@type,ctype>
			</@enum_base_field_arg>
		</#list>
	</#if>	
</#macro>

<#function make_arg_str intf mtd>
	<#local ret = "">
	<#local intf_msg = common.find_msgdef(intf.@message)>
	<#local fld = common.match_field(intf_msg, mtd.@name)>
	<@common.enum_field_args fld mtd;arg_name,type,ctype >
		<#if ret != "">
			<#local ret = ret + ", ">
		</#if>
		<#local ret = ret +ctype + " " + arg_name>
	</@common.enum_field_args>
	<#return ret>
</#function>