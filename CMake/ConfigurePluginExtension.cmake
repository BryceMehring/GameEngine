macro(ConfigurePluginExtension pluginName)
set_target_properties(${pluginName} PROPERTIES PREFIX "")
set_target_properties(${pluginName} PROPERTIES SUFFIX ".plug")
endmacro()