///(fname) -> returns files functgion's return -> VERY SLOW
if !variable_local_exists('mymy00'+filename_remove_ext(argument0))
    execute_string('mymy00'+filename_remove_ext(argument0)+'="'+file_text_read_all('visualisers\funcs\'+argument0,' ')+'"')
return execute_string("return execute_string(mymy00"+filename_remove_ext(argument0)+")")
