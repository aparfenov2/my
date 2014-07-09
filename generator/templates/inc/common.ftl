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
	<#if typedef.@type == 'complex'>
		<#list typedef.parameter as parameter>
			<#nested parameter>
		</#list>
	<#else>	
		<#stop "complex type is not complex: "+typedef.@type>
	</#if>
</#macro>

<#function ternary a b c>
	<#if a>
		<#return b>
	<#else>	
		<#return c>
	</#if>
</#function>

<#macro assert b msg=''>
	<#if !b>
		<#stop "assertion failed: " + msg>
	</#if>
</#macro>

<#macro assertEq a b msg=''>
	<#if a != b>
		<#stop "assertion "+msg+" failed: expected : " + b + ", actual: "+ a>
	</#if>
</#macro>