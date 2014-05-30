<#import "inc/common.ftl" as commons>

<#macro emit_enum typedef>
namespace ${typedef.@id} {
	typedef enum {
		<#list typedef.enum as enum>
			${enum.@id} = ${enum.@value}<#rt>
			<#lt><#if enum_has_next>,</#if>
		</#list>
	} ${typedef.@id};
}
</#macro>

<#-- Рекурсивно ищем и эмитируем составные классы -->
<#macro process_parameter parameter>
	<#local typedef = commons.find_typedef(parameter.@type)>
	
	<#if typedef.@type == 'complex'>
	
		<@commons.enumerate_complex_type typedef; child>
			<@process_parameter child />
		</@commons.enumerate_complex_type>
		
		
	<#elseif typedef.@type == 'enum'>
		<@emit_enum typedef/>
	</#if>
</#macro>

<#-- ======================== MAIN =================================== -->

#ifndef _ENUMS_H
#define _ENUMS_H
// Генерируем все параметры, определяем их тип
<#list schema.schema.parameters.parameter as parameter>
	<@process_parameter parameter />
</#list>
#endif