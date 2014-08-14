
<#macro print_tabs level>
	<#if level gt 0>
		<#t><#list 0..level as lev>${"\t"}</#list>
	</#if>
</#macro>


<#macro print_attrs node>
	<#list node.@@ as attr>
		<#local val = attr?string>
		<#local translated = node["@${attr.@@qname}_${language}"]>
		<#if language != "ru" && translated?has_content>
			<#local val = translated>
		</#if>
		<#t> ${attr.@@qname} = "${val}"<#t>
	</#list>
</#macro>

<#macro print_node node level left right>
	<@print_tabs level/>
	<#t>${left}${node.@@qname}
	<#if ! left?starts_with("</")>
		<@print_attrs node/>
	</#if>
	<#t>${right}
</#macro>

<#macro process_node node level>
	<#if node?node_name == 'dictionaries'>
	<#else>
		<#if node.*?size gt 0>
			<@print_node node level "<" ">\n" />
			<#list node.* as child>
				<@process_node child level+1/>
			</#list>
			<@print_node node level "</" ">\n" />
		<#else>
			<#if node?string?length == 0>
				<@print_node node level "<" "/>\n" />
			<#else>
				<@print_node node level "<" ">" />
				<#t>${node.@@nested_markup?trim}
				<@print_node node 0 "</" ">\n" />
			</#if>
		</#if>
	</#if>
</#macro>

