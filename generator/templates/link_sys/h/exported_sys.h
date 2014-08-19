<#import "../inc/link_sys_utils.ftl" as common>

<#macro emit_interace intf>
class ${intf.@name} {
public:
	<#list intf.method as mtd>
	virtual void ${mtd.@name} (${common.make_arg_str(intf, mtd)}) = 0;		
	</#list>	
};

</#macro>
/*
* Определение интерфейса связи системного уровня для прямой и обратной связи с этой системой
* АВТОМАТИЧЕСКИ СГЕНЕРИРОВАННЫЙ ФАЙЛ !
*/

#ifndef _MESSAGES_SYS_H
#define _MESSAGES_SYS_H


#include "types.h"

namespace link {

<#list proto.proto.interfaces.interface as intf>
	<@emit_interace intf />
</#list>




// интерфес хоста со стороны локальной системы
// для получения реальных интерфейсов использовать dynamic_cast

class host_system_interface_t {
public:
	virtual void use_dynamic_cast_on_it() {
	};

};

// интерфейс локальной системы со стороны хоста
// для получения реальных интерфейсов использовать dynamic_cast
class exported_system_interface_t {
public:

	virtual void use_dynamic_cast_on_it() {
	};

};

} // ns

#endif
