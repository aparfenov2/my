<#import "../inc/common.ftl" as common>
<#import "inc/link_utils.ftl" as utils>


<#macro emit_enum typedef>
	<@common.assert typedef.@type == 'enum' />
namespace ${typedef.@id} {
	typedef enum {
		<#list typedef.enum as enum>
			${enum.@id} = ${enum.@value}<#rt>
			<#lt><#if enum_has_next>,</#if>
		</#list>
	} ${typedef.@id};
}
</#macro>

/*
* Определение интерфейсов связи прикладного уровня для прямой и обратной связи с этой системой
*
* WARNING: АВТОМАТИЧЕСКИ СГЕНЕРИРОВАННЫЙ ФАЙЛ
*/

#ifndef _MESSAGES_APP_H
#define _MESSAGES_APP_H


#include "types.h"
#include "app_events.h"


namespace link {

<#list schema.schema.types.type as typedef>
	<#if typedef.@type == 'enum'>
	// ${typedef.@id}
		<@emit_enum typedef/>
	</#if>
</#list>


// интерфейс удаленной системы для получения сообщений
class host_interface_t {
public:
public:

<#list schema.schema.parameters.parameter as parameter>
	// ${parameter.@id}
	<@utils.emit_parameter parameter; path, type >
	virtual void ${path}_changed(${type} v) = 0;
	</@utils.emit_parameter>
</#list>
};


// интерфейс "себя" для удаленной системы
class exported_interface_t {
public:

<#list schema.schema.parameters.parameter as parameter>
	// ${parameter.@id}
	<@utils.emit_parameter parameter; path, type >
	virtual void set_${path}(${type} v) = 0;
	</@utils.emit_parameter>
</#list>
};

} // ns gen

#endif
