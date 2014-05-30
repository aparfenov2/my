<#function find_typedef id>
	<#local typedef = schema["schema/types/type[@id='"+id+"']"]>
	<#if !typedef?? || !typedef?has_content>
		<#stop "cant find typedef: "+parameter.@type>
	</#if>
	<#return typedef>
</#function>

<#function find_parameter id>
	<#local parameter = schema["schema/parameters/parameter[@id='"+id+"']"]>
	<#if !parameter?? || !parameter?has_content>
		<#stop "parameter with id=${id} not found">
	</#if>
	<#return parameter>
</#function>


<#macro enumerate_complex_type typedef>
	<#if typedef?node_name == 'group'>
		<#list typedef.parameterRef as pRef>
			<#local parameter = find_parameter(pRef.@id) >
			<#nested parameter>
		</#list>
	<#elseif typedef?node_name == 'type'>	
		<#if typedef.@type == 'complex'>
			<#list typedef.parameter as parameter>
				<#nested parameter>
			</#list>
		<#else>	
			<#stop "complex type is not complex: "+typedef.@type>
		</#if>
	<#else>
		<#stop "unknown complex type: "+typedef?node_name>
	</#if>
</#macro>
