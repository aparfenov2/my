<#import "inc/common.ftl" as commons>

<#macro emit_set_params meta tab="">
	<#list meta.@@ as attr>
		<#local isNum = true>
		<#attempt>
			<#local num = attr?number>
		<#recover>
			<#local isNum = false>
		</#attempt>
		<#if isNum>
			<#if attr?string?contains(".")>
				<#lt>${tab}->set_float_param("${attr.@@qname}",${attr?string})
			<#else>
				<#lt>${tab}->set_int_param("${attr.@@qname}",${attr?string})
			</#if>	
		<#else>
			<#lt>${tab}->set_string_param("${attr.@@qname}","${attr?string}")
		</#if>
	</#list>
</#macro>

<#macro emit_parameter_meta parameter tab="">
	<#lt>${tab}(new dynamic_parameter_meta_t())
	<@emit_set_params parameter tab+"\t"/>
</#macro>

<#macro emit_enum_meta enum tab="">
	<#lt>${tab}(new dynamic_enum_meta_t())
	<@emit_set_params enum tab+"\t"/>
</#macro>

<#macro emit_type_meta type tab="">
	<#lt>${tab}(new dynamic_type_meta_t())
	<@emit_set_params type tab+"\t"/>
	<#if type.@type == 'complex'>
		<#list type.parameter as parameter>
			<#lt>${tab+"\t"}->add_parameter(
			<@emit_parameter_meta parameter tab+"\t\t"/>
			)
		</#list>
	<#elseif type.@type == 'enum'>
		<#list type.enum as enum>
			<#lt>${tab+"\t"}->add_enum_value(
			<@emit_enum_meta enum tab+"\t\t"/>
			)
		</#list>
	</#if>
</#macro>

<#macro emit_view_meta view tab="">
	<#lt>${tab}(new dynamic_view_meta_t())
	<@emit_set_params view tab+"\t" />
	<#list view.view as child>
	<#lt>${tab}->add_child(
		<@emit_view_meta child tab+"\t"/>
		)
	</#list>
</#macro>

<#macro emit_menu_meta menu tab="">
	<#lt>${tab}(new dynamic_menu_meta_t())
	<@emit_set_params menu tab+"\t"/>
	<#list menu.parameterRef as child>
		<#lt>${tab+"\t"}->add_child("${child.@id}")
	</#list>
</#macro>

#include "generated_meta.h"

using namespace gen;

void meta_registry_t::init() {

/*
 * =================== ÒÈÏÛ ==========================
*/
	
<#list schema.schema.types.type as type>
	// ${type.@id}
	types.push_back(
	<@emit_type_meta type "\t\t" />
		);
</#list>
	
/*
 * =================== ÏÀĞÀÌÅÒĞÛ ==========================
*/
<#list schema.schema.parameters.parameter as parameter>
	// ${parameter.@id}
	parameters.push_back(
	<@emit_parameter_meta parameter "\t\t" />
		);
</#list>
	
/*
 * =================== ÂÈÄÛ ==========================
*/

<#list schema.schema.views.view as view>
	// ${view.@id}
	views.push_back(
	<@emit_view_meta view "\t\t"/>
		);
</#list>

/*
 * =================== ÌÅÍŞ ==========================
*/

<#list schema.schema.menus.menu as menu>
	// ${menu.@id}
	menus.push_back(
	<@emit_menu_meta menu "\t\t"/>
		);
</#list>

}