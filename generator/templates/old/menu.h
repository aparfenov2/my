/*
Сгенерированные описания виджетов меню
*/
<#import "inc/common.ftl" as commons>

<#-- ======================== COMMON =================================== -->

<#function get_widget_class parameter>
	<#local typedef = commons.find_typedef(parameter.@type)>
	<#if typedef.@type == 'enum'>
		<#return "combobox_t">
	<#elseif typedef.@type == 'base'>
		<#return "textbox_t">
	</#if>
	<#return "${parameter.@id}_ui_t">
</#function>


<#-- ======================== EMITTERS =================================== -->

<#macro emit_declaration parameter>
	<#local widget_class = get_widget_class(parameter)>
	${widget_class} ${parameter.@id};
</#macro>

<#macro emit_next_all_row last_param parameter>
		<#if !last_param?has_content>
		if (!prev) return &${parameter.@id};
		<#else>
		else if (prev == &${last_param.@id}) return &${parameter.@id};
		</#if>
</#macro>


<#macro emit_complex_class super name typedef>
class ${name}_ui_t : public ${super} {
	typedef ${super} super;
public:
	<@commons.enumerate_complex_type typedef; child>
		<@emit_declaration child />
	</@commons.enumerate_complex_type>
	
	stack_layout_t stack_layout;
	preferred_stack_layout_t preferred_stack_layout;
public:
	${name}_ui_t() {
		layout = &stack_layout;
		preferred_layout = &preferred_stack_layout;
	}
	
	virtual gobject_t* next_all(void* prev) OVERRIDE {
	<@commons.enumerate_complex_type typedef; child>
		<@emit_next_all_row last_param!'' child />
		<#local last_param = child>
	</@commons.enumerate_complex_type>
		return 0;
	}
};
</#macro>

<#-- Рекурсивно ищем и эмитируем составные классы -->
<#macro process_parameter parameter>
	<#local typedef = commons.find_typedef(parameter.@type)>
	
	<#if typedef.@type == 'complex'>
	
		<@commons.enumerate_complex_type typedef; child>
			<@process_parameter child />
		</@commons.enumerate_complex_type>
		
		<@emit_complex_class 'menu_item_t' parameter.@id typedef/>
		
	</#if>
</#macro>

<#-- ======================== MAIN =================================== -->
#ifndef _MENU_H
#define _MENU_H

// Генерируем все параметры, определяем их тип
<#list schema.schema.parameters.parameter as parameter>
	<@process_parameter parameter />
</#list>

// Генерируем группы меню
<#list schema.schema.groups.group as group>
	<@emit_complex_class 'menu_t' group.@id group/>
</#list>
#endif
