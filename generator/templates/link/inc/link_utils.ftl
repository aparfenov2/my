<#import "../../inc/common.ftl" as common>

<#function match_native_type id>
	<#if id=='float'><#return 'double'>
	<#elseif id=='string'><#return 'char *'>
	</#if>
	<#return id>
</#function>

<#function match_variant_type id>
	<#local id = match_native_type(id)>
	<#if ['u8','s8','u16', 's16', 'u32']?seq_contains(id)>
		<#local id = 's32'>
	</#if>
	<#return id>
</#function>

<#macro enumerate_parameter parameter path>
	<#local path = path + common.ternary(path?length > 0 '_' '') + parameter.@id>
	<#local typedef = common.find_typedef(parameter.@type)>
	<#if typedef.@type == 'complex'>
		<@common.enumerate_complex_type typedef; child>
			<@enumerate_parameter child path; ch,pth>
				<#nested ch pth>
			</@enumerate_parameter>
		</@common.enumerate_complex_type>
	<#else>
		<#nested parameter path>
	</#if>
</#macro>

<#macro resolve_param_type parameter>
	<#local typedef = common.find_typedef(parameter.@type)>
	<@common.assert typedef.@type == 'base' || typedef.@type == 'enum' />
	<#if typedef.@type == 'base'>
		<#local type = match_native_type(typedef.@id)>
	<#elseif typedef.@type == 'enum'>
		<#local type = '${typedef.@id}::${typedef.@id}'>
	</#if>
	<#nested type typedef>
</#macro>

<#macro emit_parameter param>
	<@enumerate_parameter param ''; parameter, path>
		<@resolve_param_type parameter; type, typedef>
			<#nested path type typedef>
		</@resolve_param_type>
	</@enumerate_parameter>
</#macro>

