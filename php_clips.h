#ifndef PHP_CLIPS_H
#define PHP_CLIPS_H

extern zend_module_entry clips_module_entry;
#define phpext_clips_ptr &clips_module_entry

#ifdef PHP_WIN32
#define PHP_CLIPS_API __declspec(dllexport)
#else
#define PHP_CLIPS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

int dataobj_to_zval(DATA_OBJECT *obj, zval *var);
int dataobj_or_facts_to_zval(DATA_OBJECT *obj, zval *var, void *fact_deftemplate);
void zval_to_dataobj(zval *php_val, DATA_OBJECT *obj);

PHP_MINFO_FUNCTION(clips);
PHP_MINIT_FUNCTION(clips);
PHP_RINIT_FUNCTION(clips);

PHP_FUNCTION(clips_function_call);
PHP_FUNCTION(clips_batch);
PHP_FUNCTION(clips_load);
PHP_FUNCTION(clips_bload);
PHP_FUNCTION(clips_bsave);
PHP_FUNCTION(clips_build);
PHP_FUNCTION(clips_eval);
PHP_FUNCTION(clips_save);
PHP_FUNCTION(clips_get_auto_float_dividend);
PHP_FUNCTION(clips_get_dynamic_constraint_checking);
PHP_FUNCTION(clips_get_static_constraint_checking);
PHP_FUNCTION(clips_get_sequence_operator_recognition);
PHP_FUNCTION(clips_set_auto_float_dividend);
PHP_FUNCTION(clips_set_dynamic_constraint_checking);
PHP_FUNCTION(clips_set_sequence_operator_recognition);
PHP_FUNCTION(clips_set_static_constraint_checking);
PHP_FUNCTION(clips_reset);
PHP_FUNCTION(clips_clear);
PHP_FUNCTION(clips_run);
PHP_FUNCTION(clips_dribble_active);
PHP_FUNCTION(clips_dribble_off);
PHP_FUNCTION(clips_dribble_on);
PHP_FUNCTION(clips_get_watch_item);
PHP_FUNCTION(clips_watch);
PHP_FUNCTION(clips_unwatch);
PHP_FUNCTION(clips_deftemplate_module);
PHP_FUNCTION(clips_get_deftemplate_list);
PHP_FUNCTION(clips_is_deftemplate_deletable);
PHP_FUNCTION(clips_undeftemplate);
PHP_FUNCTION(clips_get_fact_list);
PHP_FUNCTION(clips_assert);
PHP_FUNCTION(clips_get_fact_duplication);
PHP_FUNCTION(clips_get_fact_list_changed);
PHP_FUNCTION(clips_load_facts);
PHP_FUNCTION(clips_load_facts_from_string);
PHP_FUNCTION(clips_save_facts);
PHP_FUNCTION(clips_set_fact_duplication);
PHP_FUNCTION(clips_set_fact_list_changed);
PHP_FUNCTION(clips_deffacts_module);
PHP_FUNCTION(clips_get_deffacts_list);
PHP_FUNCTION(clips_is_deffacts_deletable);
PHP_FUNCTION(clips_undeffacts);
PHP_FUNCTION(clips_refresh);
PHP_FUNCTION(clips_remove_break);
PHP_FUNCTION(clips_set_break);
PHP_FUNCTION(clips_defrule_has_breakpoint);
PHP_FUNCTION(clips_defrule_module);
PHP_FUNCTION(clips_get_defrule_list);
PHP_FUNCTION(clips_get_incremental_reset);
PHP_FUNCTION(clips_set_incremental_reset);
PHP_FUNCTION(clips_is_defrule_deletable);
PHP_FUNCTION(clips_undefrule);
PHP_FUNCTION(clips_clear_focus_stack);
PHP_FUNCTION(clips_get_agenda_changed);
PHP_FUNCTION(clips_set_agenda_changed);
PHP_FUNCTION(clips_get_focus);
PHP_FUNCTION(clips_get_focus_stack);
PHP_FUNCTION(clips_get_salience_evaluation);
PHP_FUNCTION(clips_set_salience_evaluation);
PHP_FUNCTION(clips_set_strategy);
PHP_FUNCTION(clips_get_strategy);
PHP_FUNCTION(clips_pop_focus);
PHP_FUNCTION(clips_focus);
PHP_FUNCTION(clips_refresh_agenda);
PHP_FUNCTION(clips_reorder_agenda);
PHP_FUNCTION(clips_defglobal_module);
PHP_FUNCTION(clips_get_defglobal_value);
PHP_FUNCTION(clips_get_globals_changed);
PHP_FUNCTION(clips_get_reset_globals);
PHP_FUNCTION(clips_is_defglobal_deletable);
PHP_FUNCTION(clips_set_defglobal_value);
PHP_FUNCTION(clips_undefglobal);
PHP_FUNCTION(clips_deffunction_module);
PHP_FUNCTION(clips_get_deffunction_list);
PHP_FUNCTION(clips_is_deffunction_deletable);
PHP_FUNCTION(clips_undeffunction);
PHP_FUNCTION(clips_defgeneric_module);
PHP_FUNCTION(clips_get_defgeneric_list);
PHP_FUNCTION(clips_is_defgeneric_deletable);
PHP_FUNCTION(clips_undefgeneric);
PHP_FUNCTION(clips_class_abstract);
PHP_FUNCTION(clips_class_reactive);
PHP_FUNCTION(clips_class_slots);
PHP_FUNCTION(clips_class_subclasses);
PHP_FUNCTION(clips_class_superclasses);
PHP_FUNCTION(clips_defclass_module);
PHP_FUNCTION(clips_get_class_defaults_mode);
PHP_FUNCTION(clips_set_class_defaults_mode);
PHP_FUNCTION(clips_get_defclass_list);
PHP_FUNCTION(clips_is_defclass_deletable);
PHP_FUNCTION(clips_subclass);
PHP_FUNCTION(clips_superclass);
PHP_FUNCTION(clips_undefclass);
PHP_FUNCTION(clips_get_instances_changed);
PHP_FUNCTION(clips_set_instances_changed);
PHP_FUNCTION(clips_get_instance_list);
PHP_FUNCTION(clips_create_instance);
PHP_FUNCTION(clips_make_instance);
PHP_FUNCTION(clips_get_defmodule_list);
PHP_FUNCTION(clips_set_current_module);



/* CLIPS Related Macros */

#define RETURN_DATA_OBJECT(obj) dataobj_or_facts_to_zval(&obj, return_value, NULL)
#define RETURN_DATA_OBJECT_P(obj) dataobj_or_facts_to_zval(obj, return_value, NULL)
#define RETURN_FACT_LIST(obj, dt) dataobj_or_facts_to_zval(&obj, return_value, dt)
#define RETURN_FACT_LIST_P(obj, dt) dataobj_or_facts_to_zval(obj, return_value, dt)


#ifdef ZTS
#define CLIPS_G(v) TSRMG(clips_globals_id, zend_clips_globals *, v)
#else
#define CLIPS_G(v) (clips_globals.v)
#endif

#endif
