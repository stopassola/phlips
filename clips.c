
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "clipssrc/clips.h"
#include "clipssrc/tmpltutl.h"
#include "php_clips.h"

#define VERSION "0.5.1"


/* {{{ Function Entry - Register functions in PHP */
zend_function_entry clips_functions[] = {
	PHP_FE(clips_function_call, NULL)
	PHP_FE(clips_batch, NULL)
	PHP_FE(clips_load, NULL)
	PHP_FE(clips_bload, NULL)
	PHP_FE(clips_bsave, NULL)
	PHP_FE(clips_build, NULL)
	PHP_FE(clips_eval, NULL)
	PHP_FE(clips_save, NULL)
	PHP_FE(clips_get_auto_float_dividend, NULL)
	PHP_FE(clips_get_dynamic_constraint_checking, NULL)
	PHP_FE(clips_get_static_constraint_checking, NULL)
	PHP_FE(clips_get_sequence_operator_recognition, NULL)
	PHP_FE(clips_set_auto_float_dividend, NULL)
	PHP_FE(clips_set_dynamic_constraint_checking, NULL)
	PHP_FE(clips_set_sequence_operator_recognition, NULL)
	PHP_FE(clips_set_static_constraint_checking, NULL)
	PHP_FE(clips_reset, NULL)
	PHP_FE(clips_clear, NULL)
	PHP_FE(clips_run, NULL)
	PHP_FE(clips_dribble_active, NULL)
	PHP_FE(clips_dribble_off, NULL)
	PHP_FE(clips_dribble_on, NULL)
	PHP_FE(clips_get_watch_item, NULL)
	PHP_FE(clips_watch, NULL)
	PHP_FE(clips_unwatch, NULL)
	PHP_FE(clips_deftemplate_module, NULL)
	PHP_FE(clips_get_deftemplate_list, NULL)
	PHP_FE(clips_is_deftemplate_deletable, NULL)
	PHP_FE(clips_undeftemplate, NULL)
	PHP_FE(clips_get_fact_list, NULL)
	PHP_FE(clips_assert, NULL)
	PHP_FE(clips_get_fact_duplication, NULL)
	PHP_FE(clips_get_fact_list_changed, NULL)
	PHP_FE(clips_load_facts, NULL)
	PHP_FE(clips_load_facts_from_string, NULL)
	PHP_FE(clips_save_facts, NULL)
	PHP_FE(clips_set_fact_duplication, NULL)
	PHP_FE(clips_set_fact_list_changed, NULL)
	PHP_FE(clips_deffacts_module, NULL)
	PHP_FE(clips_get_deffacts_list, NULL)
	PHP_FE(clips_is_deffacts_deletable, NULL)
	PHP_FE(clips_undeffacts, NULL)
	PHP_FE(clips_refresh, NULL)
	PHP_FE(clips_remove_break, NULL)
	PHP_FE(clips_set_break, NULL)
	PHP_FE(clips_defrule_has_breakpoint, NULL)
	PHP_FE(clips_defrule_module, NULL)
	PHP_FE(clips_get_defrule_list, NULL)
	PHP_FE(clips_get_incremental_reset, NULL)
	PHP_FE(clips_set_incremental_reset, NULL)
	PHP_FE(clips_is_defrule_deletable, NULL)
	PHP_FE(clips_undefrule, NULL)
	PHP_FE(clips_clear_focus_stack, NULL)
	PHP_FE(clips_get_agenda_changed, NULL)
	PHP_FE(clips_set_agenda_changed, NULL)
	PHP_FE(clips_get_focus, NULL)
	PHP_FE(clips_get_focus_stack, NULL)
	PHP_FE(clips_get_salience_evaluation, NULL)
	PHP_FE(clips_set_salience_evaluation, NULL)
	PHP_FE(clips_set_strategy, NULL)
	PHP_FE(clips_get_strategy, NULL)
	PHP_FE(clips_pop_focus, NULL)
	PHP_FE(clips_focus, NULL)
	PHP_FE(clips_refresh_agenda, NULL)
	PHP_FE(clips_reorder_agenda, NULL)
	PHP_FE(clips_defglobal_module, NULL)
	PHP_FE(clips_get_defglobal_value, NULL)
	PHP_FE(clips_get_globals_changed, NULL)
	PHP_FE(clips_get_reset_globals, NULL)
	PHP_FE(clips_is_defglobal_deletable, NULL)
	PHP_FE(clips_set_defglobal_value, NULL)
	PHP_FE(clips_undefglobal, NULL)
	PHP_FE(clips_deffunction_module, NULL)
	PHP_FE(clips_get_deffunction_list, NULL)
	PHP_FE(clips_is_deffunction_deletable, NULL)
	PHP_FE(clips_undeffunction, NULL)
	PHP_FE(clips_defgeneric_module, NULL)
	PHP_FE(clips_get_defgeneric_list, NULL)
	PHP_FE(clips_is_defgeneric_deletable, NULL)
	PHP_FE(clips_undefgeneric, NULL)
	PHP_FE(clips_class_abstract, NULL)
	PHP_FE(clips_class_reactive, NULL)
	PHP_FE(clips_class_slots, NULL)
	PHP_FE(clips_class_subclasses, NULL)
	PHP_FE(clips_class_superclasses, NULL)
	PHP_FE(clips_defclass_module, NULL)
	PHP_FE(clips_get_class_defaults_mode, NULL)
	PHP_FE(clips_set_class_defaults_mode, NULL)
	PHP_FE(clips_get_defclass_list, NULL)
	PHP_FE(clips_is_defclass_deletable, NULL)
	PHP_FE(clips_subclass, NULL)
	PHP_FE(clips_superclass, NULL)
	PHP_FE(clips_undefclass, NULL)
	PHP_FE(clips_get_instances_changed, NULL)
	PHP_FE(clips_set_instances_changed, NULL)
	PHP_FE(clips_get_instance_list, NULL)
	PHP_FE(clips_create_instance, NULL)
	PHP_FE(clips_make_instance, NULL)
	PHP_FE(clips_get_defmodule_list, NULL)
	PHP_FE(clips_set_current_module, NULL)
	{NULL, NULL, NULL}
};
/* }}} */



/* {{{ Module Entry - Register the extension in PHP */
zend_module_entry clips_module_entry = {
	STANDARD_MODULE_HEADER,
	"clips",
	clips_functions,
	PHP_MINIT(clips),
	NULL,
	PHP_RINIT(clips),
	NULL,
	PHP_MINFO(clips),
	VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CLIPS
ZEND_GET_MODULE(clips)
#endif



/* {{{ Module Initialization Function -
   Is called when the webserver starts */
PHP_MINIT_FUNCTION(clips) {

	/* Register CLIPS Constants */

	/* Conflict Resolution Strategies */
	REGISTER_LONG_CONSTANT("DEPTH_STRATEGY", DEPTH_STRATEGY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BREADTH_STRATEGY", BREADTH_STRATEGY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("LEX_STRATEGY", LEX_STRATEGY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("MEA_STRATEGY", MEA_STRATEGY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("COMPLEXITY_STRATEGY", COMPLEXITY_STRATEGY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SIMPLICITY_STRATEGY", SIMPLICITY_STRATEGY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("RANDOM_STRATEGY", RANDOM_STRATEGY, CONST_CS | CONST_PERSISTENT);
	
	/* Save Scopes */
	REGISTER_LONG_CONSTANT("LOCAL_SAVE", LOCAL_SAVE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("VISIBLE_SAVE", VISIBLE_SAVE, CONST_CS | CONST_PERSISTENT);

	/* Salience Evaluation Modes */
	REGISTER_LONG_CONSTANT("WHEN_DEFINED", WHEN_DEFINED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("WHEN_ACTIVATED", WHEN_ACTIVATED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("EVERY_CYCLE", EVERY_CYCLE, CONST_CS | CONST_PERSISTENT);

	/* Class Default Modes */
	REGISTER_LONG_CONSTANT("CONVENIENCE_MODE", CONVENIENCE_MODE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("CONSERVATION_MODE", CONSERVATION_MODE, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */



/* {{{ Request Initialization Function -
   Is called upon each page request */
PHP_RINIT_FUNCTION(clips) {

	/* Initialize a CLIPS environment */
	InitializeEnvironment();
	Clear();

	return SUCCESS;
}
/* }}} */



/* {{{ Module Info Function -
   Defines information about the extension to be listed when 
   phpinfo() is called */
PHP_MINFO_FUNCTION(clips) {
	php_info_print_table_start();
	php_info_print_table_row(2, "CLIPS Support", "enabled");
	php_info_print_table_row(2, "CLIPS Extension Version", VERSION);
	php_info_print_table_row(2, "CLIPS Version", "6.2");
	php_info_print_table_end();
}
/* }}} */




/* INTERNAL FUNCTIONS */

/* {{{ Convert a CLIPS DATA_OBJECT into a zval */
int dataobj_to_zval(DATA_OBJECT *obj, zval *var) {
	return dataobj_or_facts_to_zval(obj, var, NULL);
}

int dataobj_or_facts_to_zval(DATA_OBJECT *obj, zval *var, void *fact_deftemplate) {
	int i, i2, i3, end, end2, end3, is_ordered_fact;
	void *multifield_ptr, *multifield_ptr2, *multifield_ptr3, *fact_ptr, *instance_ptr, *defclass_ptr, *fact_dt_ptr;
	DATA_OBJECT slot_names, slot_value;
	zval *fact, *instance, *multi_val, *multislot_val;

	switch (GetpType(obj)) {
		case FLOAT: 
			ZVAL_DOUBLE(var, DOPToDouble(obj));
			return 1;
			break;
		
		case STRING: case SYMBOL: case INSTANCE_NAME: 
			ZVAL_STRING(var, DOPToString(obj), 1);
			return 1;
			break;
		
		case INTEGER: 
			ZVAL_LONG(var, DOPToLong(obj));
			return 1;
			break;
		
		case FACT_ADDRESS:
			/*ptr = DOPToPointer(obj);*/
			ZVAL_STRING(var, "fact-address", 1);
			break;

		case INSTANCE_ADDRESS:
			/*ptr = DOPToPointer(obj);*/
			ZVAL_STRING(var, "instance-address", 1);
			break;

		case EXTERNAL_ADDRESS:
			/*ptr = DOPToPointer(obj);*/
			ZVAL_STRING(var, "external-address", 1);
			break;

		case MULTIFIELD:
			array_init(var);
			end = GetpDOEnd(obj);
			multifield_ptr = GetpValue(obj);

			for (i = GetpDOBegin(obj); i <= end; i++)
			{
				switch (GetMFType(multifield_ptr, i))
				{
					case FLOAT: case INTEGER_OR_FLOAT:
						add_next_index_double(var, ValueToDouble(GetMFValue(multifield_ptr, i)));
						break;

					case STRING: case SYMBOL: case SYMBOL_OR_STRING: case INSTANCE_NAME: case INSTANCE_OR_INSTANCE_NAME:
						add_next_index_string(var, ValueToString(GetMFValue(multifield_ptr, i)), 1);
						break;

					case INTEGER:
						add_next_index_long(var, ValueToLong(GetMFValue(multifield_ptr, i)));
						break;

					case FACT_ADDRESS:
						is_ordered_fact = 0;
						MAKE_STD_ZVAL(fact);
						array_init(fact);
						fact_ptr = GetMFValue(multifield_ptr, i);
						fact_dt_ptr = FactDeftemplate(fact_ptr);

						if (fact_deftemplate == NULL || fact_dt_ptr == fact_deftemplate) {
							FactSlotNames(fact_ptr, &slot_names);

							end2 = GetDOEnd(slot_names);
							multifield_ptr2 = GetValue(slot_names);

							/* Iterate through the fact slots and retrieve the values in each slot */
							for (i2 = GetDOBegin(slot_names); i2 <= end2; i2++) {
								/* Store the current slot value into a new DATA_OBJECT */
								if (end2 == 1 && strcmp("implied", ValueToString(GetMFValue(multifield_ptr2, i2))) == 0) {
									
									GetFactSlot(fact_ptr, NULL, &slot_value);

									MAKE_STD_ZVAL(multi_val);
									array_init(multi_val);

									add_next_index_string(multi_val, GetDeftemplateName(fact_dt_ptr), 1);

									end3 = GetDOEnd(slot_value);
									multifield_ptr3 = GetValue(slot_value);

									for (i3 = GetDOBegin(slot_value); i3 <= end3; i3++) {
										switch (GetMFType(multifield_ptr3, i3)) {
											case FLOAT: case INTEGER_OR_FLOAT:
												add_next_index_double(multi_val, ValueToDouble(GetMFValue(multifield_ptr3, i3)));
												break;

											case STRING: case SYMBOL: case SYMBOL_OR_STRING: case INSTANCE_NAME: case INSTANCE_OR_INSTANCE_NAME:
												add_next_index_string(multi_val, ValueToString(GetMFValue(multifield_ptr3, i3)), 1);
												break;

											case INTEGER:
												add_next_index_long(multi_val, ValueToLong(GetMFValue(multifield_ptr3, i3)));
												break;
											
											case MULTIFIELD:
												MAKE_STD_ZVAL(multislot_val);
												dataobj_to_zval(&slot_value, multislot_val);
												add_assoc_zval(fact, ValueToString(GetMFValue(multifield_ptr3, i3)), multislot_val);
												break;
											
											default:
												add_next_index_string(multi_val, "(unknown value)", 1);
												break;
										}
									}
									
									add_next_index_zval(var, multi_val);
									is_ordered_fact = 1;
								}

								else {
									GetFactSlot(fact_ptr, ValueToString(GetMFValue(multifield_ptr2, i2)), &slot_value);
								
									/* Convert the DATA_OBJECT into a PHP data type and append to the fact array */
									switch (GetType(slot_value)) {
										case FLOAT: 
											add_assoc_double(fact, ValueToString(GetMFValue(multifield_ptr2, i2)), DOToDouble(slot_value));
											break;
										
										case STRING: case SYMBOL: case INSTANCE_NAME: 
											add_assoc_string(fact, ValueToString(GetMFValue(multifield_ptr2, i2)), DOToString(slot_value), 1);
											break;
										
										case INTEGER: 
											add_assoc_long(fact, ValueToString(GetMFValue(multifield_ptr2, i2)), DOToLong(slot_value));
											break;

										case MULTIFIELD:
											MAKE_STD_ZVAL(multislot_val);
											dataobj_to_zval(&slot_value, multislot_val);
											add_assoc_zval(fact, ValueToString(GetMFValue(multifield_ptr2, i2)), multislot_val);
											break;
										
										default:
											add_assoc_string(fact, ValueToString(GetMFValue(multifield_ptr2, i2)), "(unknown value)", 1);
											break;
									}
								}
							}

							/* Add the fact array to the var array */
							if ( ! is_ordered_fact) {
								add_next_index_zval(var, fact);
							}
						}

						break;

					case INSTANCE_ADDRESS:
						MAKE_STD_ZVAL(instance);
						array_init(instance);
						instance_ptr = GetMFValue(multifield_ptr, i);

						defclass_ptr = GetInstanceClass(instance_ptr);

						/* Get the slot names of the defclass */
						ClassSlots(defclass_ptr, &slot_names, 1);

						end2 = GetDOEnd(slot_names);
						multifield_ptr2 = GetValue(slot_names);

						/* Iterate through the instance slots and retrieve the values in each slot */
						for (i2 = GetDOBegin(slot_names); i2 <= end2; i2++) {
					
							/* Store the current slot value into a new DATA_OBJECT */
							DirectGetSlot(instance_ptr, ValueToString(GetMFValue(multifield_ptr2, i2)), &slot_value);

							/* Convert the DATA_OBJECT into a PHP data type and append to the instance array */
							switch (GetType(slot_value)) {
								case FLOAT: 
									add_assoc_double(instance, ValueToString(GetMFValue(multifield_ptr2, i2)), DOToDouble(slot_value));
									break;
								
								case STRING: case SYMBOL: case INSTANCE_NAME: 
									add_assoc_string(instance, ValueToString(GetMFValue(multifield_ptr2, i2)), DOToString(slot_value), 1);
									break;
								
								case INTEGER: 
									add_assoc_long(instance, ValueToString(GetMFValue(multifield_ptr2, i2)), DOToLong(slot_value));
									break;
							}
						}

						/* Add the instance array to the var array */
						add_next_index_zval(var, instance);

						break;

					case MULTIFIELD:
						add_next_index_string(var, "(multifield)", 1);
						break;
					
					case EXTERNAL_ADDRESS:
						add_next_index_string(var, "(external-address)", 1);
						break;
					
					default:
						add_next_index_string(var, "(unknown-value)", 1);
						break;
				
				} /* End switch */
			
			} /* End for */
			
			return 1;
			break;

		default:
			ZVAL_STRING(var, "(unknown-value)", 1);
			break;
	}

	return 0;
}
/* }}} */



/* {{{ Convert a zval into a CLIPS DATA_OBJECT */
void zval_to_dataobj(zval *php_val, DATA_OBJECT *obj) {
	void *tmp_val;
	int arr_count = 0;
	zval **val;

	switch (Z_TYPE_P(php_val)) {
		case IS_STRING: 
			SetpType(obj, STRING);
			tmp_val = AddSymbol(Z_STRVAL_P(php_val));
			SetpValue(obj, tmp_val);
		break;

		case IS_BOOL:
			SetpType(obj, SYMBOL);

			if (Z_LVAL_P(php_val)) {
				tmp_val = AddSymbol("TRUE");
			} else {
				tmp_val = AddSymbol("FALSE");
			}

			SetpValue(obj, tmp_val);
		break;
		
		case IS_LONG: 
			SetpType(obj, INTEGER);
			tmp_val = AddLong(Z_LVAL_P(php_val));
			SetpValue(obj, tmp_val);
		break;
		
		case IS_DOUBLE:
			SetpType(obj, FLOAT);
			tmp_val = AddDouble(Z_DVAL_P(php_val));
			SetpValue(obj, tmp_val);
		break;
		
		case IS_ARRAY: 
			while (zend_hash_get_current_data(Z_ARRVAL_P(php_val), (void **)&val) == SUCCESS) { 
				switch (Z_TYPE_PP(val)) {
					case IS_STRING: case IS_LONG: case IS_DOUBLE:
						arr_count++;
						break;
				}
				
				zend_hash_move_forward(Z_ARRVAL_P(php_val));
			}

			tmp_val = CreateMultifield(arr_count);
			arr_count = 1;

			zend_hash_internal_pointer_reset(Z_ARRVAL_P(php_val));
			
			while (zend_hash_get_current_data(Z_ARRVAL_P(php_val), (void **)&val) == SUCCESS) { 
				SEPARATE_ZVAL(val);
				
				switch (Z_TYPE_PP(val)) {
					case IS_STRING:
						SetMFType(tmp_val, arr_count, STRING);
						SetMFValue(tmp_val, arr_count, AddSymbol(Z_STRVAL_PP(val)));
					break;
					
					case IS_LONG: 
						SetMFType(tmp_val, arr_count, INTEGER);
						SetMFValue(tmp_val, arr_count, AddLong(Z_LVAL_PP(val)));
					break;
					
					case IS_DOUBLE: 
						SetMFType(tmp_val, arr_count, FLOAT);
						SetMFValue(tmp_val, arr_count, AddDouble(Z_DVAL_PP(val)));
					break;
					
					default: 
						arr_count--; 
					break;
				}

				arr_count++;
				zend_hash_move_forward(Z_ARRVAL_P(php_val));
			}

			SetpType(obj, MULTIFIELD);
			SetpValue(obj, tmp_val);
			SetpDOBegin(obj, 1);
			SetpDOEnd(obj, (arr_count-1));

		break;

		default:
			zend_error(E_WARNING, "Cannot convert objects or resources into a CLIPS data object");
		break;
	}
}
/* }}} */









/* ENVIRONMENT FUNCTIONS */



/* {{{ proto bool clips_function_call(string function_name [, string arguments])
   Allows CLIPS system functions, deffunctions and generic functions to be called
   from PHP */
PHP_FUNCTION(clips_function_call) {
	char *function_name, *arguments;
	int function_name_len, arguments_len;
	DATA_OBJECT result;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, 
		"s|s", 
		&function_name, 
		&function_name_len, 
		&arguments, 
		&arguments_len) == FAILURE)
	{
        RETURN_FALSE;
    }

	if (ZEND_NUM_ARGS() > 1) {
		FunctionCall(function_name, arguments, &result);
	}

	else {
		FunctionCall(function_name, NULL, &result);
	}

	RETURN_DATA_OBJECT(result);
}
/* }}} */



/* {{{ bool clips_batch(string file) 
   Evaluate a series of commands stored in the specified file. Equivalent of the 
   CLIPS batch* command. */
PHP_FUNCTION(clips_batch) {
	char *file;
	int file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (BatchStar(file)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to load CLIPS batch file '%s'", file);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_load(string file); 
   Load a set of constructs into CLIPS from a file */
PHP_FUNCTION(clips_load) {
	char *file;
	int file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Load(file)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "failed to load clips source file '%s'", file);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_bload(string file); 
   Load a set of constructs into CLIPS from a binary CLIPS file */
PHP_FUNCTION(clips_bload) {
	char *file;
	int file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Bload(file)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to load clips binary file '%s'", file);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_bsave(string file) 
   Save a binary image of constructs currently in the CLIPS environment. */
PHP_FUNCTION(clips_bsave) {
	char *file;
	int file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Bsave(file)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to save clips binary file '%s'", file);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_build(string construct_string)
   Allows a construct to be defined */
PHP_FUNCTION(clips_build) {
	char *construct_str;
	int construct_str_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &construct_str, &construct_str_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Build(construct_str)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to define CLIPS construct");
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_eval(string expression)
   Allows an expression to be evaluated */
PHP_FUNCTION(clips_eval) {
	char *expr;
	int expr_len;
	DATA_OBJECT result;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &expr, &expr_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Eval(expr, &result)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to evaluate CLIPS expression");
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_save(string filename)
   Saves a set of constructs to the specified file */
PHP_FUNCTION(clips_save) {
	char* filename;
	int filename_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Save(filename)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to save CLIPS construct file '%s'", filename);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_get_auto_float_dividend()
   Get the current value of the auto-float dividend behavior. Equivalent of
   the CLIPS get-auto-float-dividend command. */
PHP_FUNCTION(clips_get_auto_float_dividend) {
	if (GetAutoFloatDividend()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_get_dynamic_constraint_checking()
   Get the current value of the dynamic constraint checking behavior. Equivalent of
   the CLIPS get-dynamic-constraint-checking command. */
PHP_FUNCTION(clips_get_dynamic_constraint_checking) {
	if (GetDynamicConstraintChecking()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_get_static_constraint_checking()
   Get the current value of the static constraint checking behavior. Equivalent of
   the CLIPS get-static-constraint-checking command. */
PHP_FUNCTION(clips_get_static_constraint_checking) {
	if (GetStaticConstraintChecking()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_get_sequence_operator_recognition()
   Get the current value of the sequence operator recognition behavior. Equivalent of
   the CLIPS get-sequence-operator-recognition command. */
PHP_FUNCTION(clips_get_sequence_operator_recognition) {
	if (GetSequenceOperatorRecognition()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto void clips_set_auto_float_dividend(bool value)
   Set the auto-float dividend behavior. Equivalent of
   the CLIPS set-auto-float-dividend command. */
PHP_FUNCTION(clips_set_auto_float_dividend) {
	int val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
        RETURN_FALSE;
    }

	SetAutoFloatDividend(val);
}
/* }}} */



/* {{{ proto void clips_set_dynamic_constraint_checking(bool value)
   Set the dynamic constraint checking behavior. Equivalent of
   the CLIPS set-dynamic-constraint-checking command. */
PHP_FUNCTION(clips_set_dynamic_constraint_checking) {
	int val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
        RETURN_FALSE;
    }

	SetDynamicConstraintChecking(val);
}
/* }}} */



/* {{{ proto void clips_set_sequence_operator_recognition(bool value)
   Set the sequence operator recognition behavior. Equivalent of
   the CLIPS set-sequence-operator-recognition command. */
PHP_FUNCTION(clips_set_sequence_operator_recognition) {
	int val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
        RETURN_FALSE;
    }

	SetSequenceOperatorRecognition(val);
}
/* }}} */



/* {{{ proto void clips_set_static_constraint_checking(bool value)
   Set the static constraint checking behavior. Equivalent of
   the CLIPS set-static-constraint-checking command. */
PHP_FUNCTION(clips_set_static_constraint_checking) {
	int val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
        RETURN_FALSE;
    }

	SetStaticConstraintChecking(val);
}
/* }}} */



/* {{{ proto void clips_reset();
   Reset the CLIPS environment */
PHP_FUNCTION(clips_reset) {
	Reset();
	RETURN_TRUE;
}
/* }}} */



/* {{{ proto void clips_clear();
   Clear the CLIPS environment */
PHP_FUNCTION(clips_clear) {
	Clear();
	RETURN_TRUE;
}
/* }}} */



/* {{{ proto int clips_run([int run_limit]);
   Execute rules in the agenda, returns the number of rules that were fired. */
PHP_FUNCTION(clips_run) {
	int run_limit = -1;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &run_limit) == FAILURE) {
        RETURN_FALSE;
    }

	RETURN_LONG(Run(run_limit));
}
/* }}} */









/* DEBUGGING FUNCTIONS */



/* {{{ proto bool clips_dribble_active()
   Determines if the storing of dribble information is active */
PHP_FUNCTION(clips_dribble_active) {
	if (DribbleActive()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_dribble_off()
   Turns off the storing of dribble information */
PHP_FUNCTION(clips_dribble_off) {
	if (DribbleOff()) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "An error occurred while attempting to close dribble file");
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_dribble_on(string filename)
   Allows the dribble function of CLIPS to be turned on */
PHP_FUNCTION(clips_dribble_on) {
	char *filename;
	int filename_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (DribbleOn(filename)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "An error occurred while attempting to open dribble file '%s'", filename);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_get_watch_item(string item)
   Tell whether or not the given item is being watched */
PHP_FUNCTION(clips_get_watch_item) {
	char *item;
	int item_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &item, &item_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (GetWatchItem(item)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_watch(string item)
   Allows the tracing facilities of CLIPS to be activated */
PHP_FUNCTION(clips_watch) {
	char *item;
	int item_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &item, &item_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Watch(item)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to watch item '%s', valid items to watch are: (facts, rules, activations, focus, compilations, statistics, globals, instances, slots, messages, message-handlers, generic-functions, method, or deffunctions)", item);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_unwatch(string item)
   Allows the tracing facilities of CLIPS to be deactivated */
PHP_FUNCTION(clips_unwatch) {
	char *item;
	int item_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &item, &item_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (Unwatch(item)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to deactivate watch on item '%s'", item);
		RETURN_FALSE;
	}
}
/* }}} */









/* DEFTEMPLATE FUNCTIONS */



/* {{{ proto string clips_deftemplate_module(string deftemplate)
   Get the name of the module in which a deftemplate is defined */
PHP_FUNCTION(clips_deftemplate_module) {
	char *dt_name;
	int dt_name_len;
	void *dt_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dt_name, &dt_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dt_ptr = FindDeftemplate(dt_name);

	if (dt_ptr == NULL) {
		zend_error(E_WARNING, "Unknown deftemplate '%s'", dt_name);
		RETURN_FALSE;
	}

	RETURN_STRING(DeftemplateModule(dt_ptr), 1);
}
/* }}} */



/* {{{ proto array clips_get_deftemplate_list([string module])
   Returns a list of all deftemplates */
PHP_FUNCTION(clips_get_deftemplate_list) {
	char *module;
	int module_len = 0;
	void *module_ptr;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	/* If a module name was given, get a pointer to the module */
	if (module_len > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	} else {
		module_ptr = NULL;
	}

	GetDeftemplateList(&retval, module_ptr);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto bool clips_is_deftemplate_deletable(string deftemplate)
   Indicates whether or not a particular deftemplate can be deleted */
PHP_FUNCTION(clips_is_deftemplate_deletable) {
	char *deftemplate;
	int deftemplate_len;
	void *deftemplate_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &deftemplate, &deftemplate_len) == FAILURE) {
        RETURN_FALSE;
    }

	deftemplate_ptr = FindDeftemplate(deftemplate);

	if (deftemplate_ptr == NULL) {
		zend_error(E_WARNING, "Deftemplate '%s' does not exist", deftemplate);
		RETURN_FALSE;
	}

	if (IsDeftemplateDeletable(deftemplate_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_undeftemplate(string deftemplate)
   Removes a deftemplate from CLIPS */
PHP_FUNCTION(clips_undeftemplate) {
	char *deftemplate;
	int deftemplate_len;
	void *deftemplate_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &deftemplate, &deftemplate_len) == FAILURE) {
        RETURN_FALSE;
    }

	deftemplate_ptr = FindDeftemplate(deftemplate);

	if (deftemplate_ptr == NULL) {
		zend_error(E_WARNING, "Deftemplate '%s' does not exist", deftemplate);
		RETURN_FALSE;
	}

	if (Undeftemplate(deftemplate_ptr)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to remove deftemplate '%s'", deftemplate);
		RETURN_FALSE;
	}
}
/* }}} */









/* FACT FUNCTIONS */



/* {{{ proto array clips_get_fact_list([string type [, string module]]);
   Retrieve the fact list */
PHP_FUNCTION(clips_get_fact_list) {
	char *type, *module;
	int type_len, module_len;
	void *deftemplate_ptr, *module_ptr;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ss", &type, &type_len, &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (ZEND_NUM_ARGS() > 0 && type_len > 0) {
		deftemplate_ptr = FindDeftemplate(type);

		if (deftemplate_ptr == NULL) {
			zend_error(E_WARNING, "Unknown deftemplate or fact type '%s'", type);
			RETURN_FALSE;
		}
	} else {
		deftemplate_ptr = NULL;
	}

	if (ZEND_NUM_ARGS() > 1 && module_len > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	} else {
		module_ptr = NULL;
	}

	GetFactList(&retval, module_ptr);
	RETURN_FACT_LIST(retval, deftemplate_ptr);
}
/* }}} */



/* {{{ proto int clips_assert(array values [, string deftemplate [, string module]]);
   Assert a fact, returns the fact index of the created fact */
PHP_FUNCTION(clips_assert) {
	char *dt, *module, *key;
	int dt_len = 0, module_len = 0, errors = 0, should_rehash = 0;
	unsigned int k = 0;
	uint key_type, key_len;
	ulong idx;
	zval *values, **val;
	void *dt_ptr, *fact_ptr, *module_ptr, *prev_module_ptr, *env;
	Bucket *p;
	DATA_OBJECT slot_value, ordered_slots;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|ss", &values, &dt, &dt_len, &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	/* If the fact was given as a string, use AssertString */
	if (Z_TYPE_P(values) == IS_STRING) {
		if (module_len > 0) {
			module_ptr = FindDefmodule(module);
			
			if (module_ptr == NULL) {
				zend_error(E_WARNING, "Unknown module '%s'", module);
				RETURN_FALSE;
			}

			prev_module_ptr = GetCurrentModule();

			if (module_ptr != prev_module_ptr) {
				SetCurrentModule(module_ptr);
			}
		}

		fact_ptr = AssertString(Z_STRVAL_P(values));

		if (module_len > 0 && module_ptr != prev_module_ptr) {
			SetCurrentModule(prev_module_ptr);
		}

		if (fact_ptr == NULL) {
			zend_error(E_WARNING, "Failed to assert fact into the CLIPS environment: \"%s\"", Z_STRVAL_P(values));
			RETURN_FALSE;
		}

		RETURN_LONG(FactIndex(fact_ptr));
	}

	else if (Z_TYPE_P(values) != IS_ARRAY) {
		zend_error(E_WARNING, "The first parameter to %s must be either a string or an array", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	/* if a deftemplate was specified */
	else if (dt_len > 0) {
		dt_ptr = FindDeftemplate(dt);

		if (dt_ptr == NULL) {
			zend_error(E_WARNING, "Unknown deftemplate '%s'", dt);
			RETURN_FALSE;
		}

		fact_ptr = CreateFact(dt_ptr);

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(values));
			
		while (zend_hash_get_current_data(Z_ARRVAL_P(values), (void **)&val) == SUCCESS) {
			/* get key */
			key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &key, &key_len, &idx, 0, NULL);
			
			/* get value */
			SEPARATE_ZVAL(val);

			zval_to_dataobj(*val, &slot_value);

			/* save the current slot value */
			if ( ! PutFactSlot(fact_ptr, key, &slot_value)) {
				zend_error(E_WARNING, "Failed to save value to fact slot '%s', the slot may not exist or the value is the wrong type", key);
				errors++;
			}

			zend_hash_move_forward(Z_ARRVAL_P(values));
		}

		if (errors) {
			RETURN_FALSE;
		}

		/* switch module if necessary */
		if (module_len > 0) {
			module_ptr = FindDefmodule(module);

			if (module_ptr == NULL) {
				zend_error(E_WARNING, "Unknown module '%s'", module);
				RETURN_FALSE;
			}

			prev_module_ptr = GetCurrentModule();

			if (module_ptr != prev_module_ptr) {
				SetCurrentModule(module_ptr);
			}
		}

		/* assert the newly created fact */
		Assert(fact_ptr);

		/* switch back to previous module if necessary */
		if (module_len > 0 && module_ptr != prev_module_ptr) {
			SetCurrentModule(prev_module_ptr);
		}

		RETURN_LONG(FactIndex(fact_ptr));
	}
	
	else {
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(values));

		if (zend_hash_get_current_data(Z_ARRVAL_P(values), (void **)&val) == SUCCESS) {
			SEPARATE_ZVAL(val);
			
			if (Z_TYPE_PP(val) != IS_STRING) {
				zend_error(E_WARNING, "The first value of an ordered fact must always be a string");
				RETURN_FALSE;
			}

			dt_ptr = FindDeftemplate(Z_STRVAL_PP(val));

			if (dt_ptr == NULL) {
				env = GetCurrentEnvironment();
	
				/* if the implied deftemplate doesn't exist yet, create it */
				CreateImpliedDeftemplate(
					env, 
					(SYMBOL_HN *) EnvAddSymbol(env,Z_STRVAL_PP(val)), 
					TRUE
				);

				dt_ptr = FindDeftemplate(Z_STRVAL_PP(val));

				if (dt_ptr == NULL) {
					zend_error(E_WARNING, "Failed to create implied deftemplate '%s'", Z_STRVAL_PP(val));
				}
			}
		} else {
			RETURN_FALSE;
		}

		fact_ptr = CreateFact(dt_ptr);
		


		/* begin array_shift() code */
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(values));
		zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &key, &key_len, &idx, 0, NULL);
		zend_hash_del_key_or_index(Z_ARRVAL_P(values), key, key_len, idx, (key) ? HASH_DEL_KEY : HASH_DEL_INDEX);
		p = Z_ARRVAL_P(values)->pListHead;
		
		while (p != NULL) {
			if (p->nKeyLength == 0) {
				if (p->h != k) {
					p->h = k++;
					should_rehash = 1;
				} else {
					k++;
				}
			}
			
			p = p->pListNext;
		}
		
		Z_ARRVAL_P(values)->nNextFreeElement = k;
		
		if (should_rehash) {
			zend_hash_rehash(Z_ARRVAL_P(values));
		}

		zend_hash_internal_pointer_reset(Z_ARRVAL_P(values));
		
		/* end array_shift() code */



		zval_to_dataobj(values, &ordered_slots);
		
		if ( ! PutFactSlot(fact_ptr, NULL, &ordered_slots)) {
			zend_error(E_WARNING, "Failed to save ordered fact data");
			RETURN_FALSE;
		}


		/* switch module if necessary */
		if (module_len > 0) {
			module_ptr = FindDefmodule(module);

			if (module_ptr == NULL) {
				zend_error(E_WARNING, "Unknown module '%s'", module);
				RETURN_FALSE;
			}
		
			prev_module_ptr = GetCurrentModule();

			if (module_ptr != prev_module_ptr) {
				SetCurrentModule(module_ptr);
			}
		}

		/* assert the newly created fact */
		Assert(fact_ptr);

		/* switch back to previous module if necessary */
		if (module_len > 0 && module_ptr != prev_module_ptr) {
			SetCurrentModule(prev_module_ptr);
		}


		RETURN_LONG(FactIndex(fact_ptr));
	}
}
/* }}} */



/* {{{ proto bool clips_get_fact_duplication()
   Get the current value of the fact duplication behavior. */
PHP_FUNCTION(clips_get_fact_duplication) {
	if (GetFactDuplication()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_get_fact_list_changed()
   Determines if any changes to the fact list have occurred. */
PHP_FUNCTION(clips_get_fact_list_changed) {
	if (GetFactListChanged()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_load_facts(string file)
   Loads a set of facts from a file into the CLIPS environment. */
PHP_FUNCTION(clips_load_facts) {
	char *file;
	int file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (LoadFacts(file)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to load CLIPS facts file '%s'", file);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_load_facts_from_string(string str)
   Loads a set of facts from a string into the CLIPS environment. */
PHP_FUNCTION(clips_load_facts_from_string) {
	char *str;
	int str_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (LoadFactsFromString(str, str_len)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to load CLIPS facts string '%s'", str);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_save_facts(string file [, int save_scope])
   Loads a set of facts from a string into the CLIPS environment. */
PHP_FUNCTION(clips_save_facts) {
	char *file;
	int file_len, save_scope = LOCAL_SAVE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &file, &file_len, &save_scope) == FAILURE) {
        RETURN_FALSE;
    }

	if (SaveFacts(file, save_scope, NULL)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to save CLIPS facts to file '%s'", file);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto void clips_set_fact_duplication(bool value)
   Set the fact duplication behavior. Equivalent of the CLIPS set-fact-duplication command. */
PHP_FUNCTION(clips_set_fact_duplication) {
	int value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
        RETURN_FALSE;
    }

	SetFactDuplication(value);
}
/* }}} */



/* {{{ proto void clips_set_fact_list_changed(bool value)
   Set the internal boolean flag which indicates when changes to the fact list
   have occurred. */
PHP_FUNCTION(clips_set_fact_list_changed) {
	int value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
        RETURN_FALSE;
    }

	SetFactListChanged(value);
}
/* }}} */









/* DEFFACTS FUNCTIONS */



/* {{{ proto string clips_deffacts_module(string deffacts_name)
   Get the name of the module in which a particular deffacts is defined */
PHP_FUNCTION(clips_deffacts_module) {
	char *df_name;
	int df_name_len;
	void *df_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &df_name, &df_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	df_ptr = FindDeffacts(df_name);

	if (df_ptr == NULL) {
		zend_error(E_WARNING, "Unknown deffacts '%s'", df_name);
		RETURN_FALSE;
	}

	RETURN_STRING(DeffactsModule(df_ptr), 1);
}
/* }}} */



/* {{{ proto array clips_get_deffacts_list([string module])
   Retrieve an array of the names of the deffacts in the CLIPS environment. Limits by 
   module if a module name is given. */
PHP_FUNCTION(clips_get_deffacts_list) {
	char *module;
	int module_len;
	void *module_ptr;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (ZEND_NUM_ARGS() > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	} else {
		module_ptr = NULL;
	}

	GetDeffactsList(&retval, module_ptr);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto array clips_is_deffacts_deletable(string deffacts_name)
   Tell whether or not a particular deffacts can be deleted */
PHP_FUNCTION(clips_is_deffacts_deletable) {
	char *df_name;
	int df_name_len;
	void *df_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &df_name, &df_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	df_ptr = FindDeffacts(df_name);

	if (df_ptr == NULL) {
		zend_error(E_WARNING, "Unknown deffacts name '%s'", df_name);
		RETURN_FALSE;
	}

	if (IsDeffactsDeletable(df_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool undeffacts(string deffacts_name)
   Remove a deffacts construct from the CLIPS environment */
PHP_FUNCTION(clips_undeffacts) {
	char *df_name;
	int df_name_len;
	void *df_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &df_name, &df_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	df_ptr = FindDeffacts(df_name);

	if (df_ptr == NULL) {
		zend_error(E_WARNING, "Unknown deffacts name '%s'", df_name);
		RETURN_FALSE;
	}

	if (Undeffacts(df_ptr)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to remove deffacts '%s'", df_name);
		RETURN_FALSE;
	}
}
/* }}} */










/* DEFRULE FUNCTIONS */



/* {{{ proto bool clips_refresh(string defrule)
   Refreshes a rule */
PHP_FUNCTION(clips_refresh) {
	char *dr_name;
	int dr_name_len;
	void *dr_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dr_name, &dr_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dr_ptr = FindDefrule(dr_name);

	if (dr_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defrule '%s'", dr_name);
		RETURN_FALSE;
	}

	if (Refresh(dr_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_remove_break(string defrule)
   Removes a breakpoint for the specified defrule */
PHP_FUNCTION(clips_remove_break) {
	char *dr_name;
	int dr_name_len;
	void *dr_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dr_name, &dr_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dr_ptr = FindDefrule(dr_name);

	if (dr_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defrule '%s'", dr_name);
		RETURN_FALSE;
	}

	if (RemoveBreak(dr_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto void clips_set_break(string defrule)
   Adds a breakpoint for the specified defrule */
PHP_FUNCTION(clips_set_break) {
	char *dr_name;
	int dr_name_len;
	void *dr_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dr_name, &dr_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dr_ptr = FindDefrule(dr_name);

	if (dr_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defrule '%s'", dr_name);
		RETURN_FALSE;
	}

	SetBreak(dr_ptr);
}
/* }}} */



/* {{{ proto bool clips_defrule_has_breakpoint(string defrule)
   Tell whether or not a particular defrule has a breakpoint set */
PHP_FUNCTION(clips_defrule_has_breakpoint) {
	char *dr_name;
	int dr_name_len;
	void *dr_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dr_name, &dr_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dr_ptr = FindDefrule(dr_name);

	if (dr_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defrule '%s'", dr_name);
		RETURN_FALSE;
	}

	if (DefruleHasBreakpoint(dr_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto string clips_defrule_module(string defrule)
   Get the name of the module in which a defrule is defined */
PHP_FUNCTION(clips_defrule_module) {
	char *dr_name;
	int dr_name_len;
	void *dr_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dr_name, &dr_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dr_ptr = FindDefrule(dr_name);

	if (dr_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defrule '%s'", dr_name);
		RETURN_FALSE;
	}

	RETURN_STRING(DefruleModule(dr_ptr), 1);
}
/* }}} */



/* {{{ proto array clips_get_defrule_list([string module])
   Retrieve an array of the names of the defrules in the CLIPS environment. Limits by 
   module if a module name is given. */
PHP_FUNCTION(clips_get_defrule_list) {
	char *module;
	int module_len;
	void *module_ptr;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (ZEND_NUM_ARGS() > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	} else {
		module_ptr = NULL;
	}

	GetDefruleList(&retval, module_ptr);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto bool clips_get_incremental_reset()
   Get the current value of the incremental reset behavior. Equivalent of
   the CLIPS get-incremental-reset command. */
PHP_FUNCTION(clips_get_incremental_reset) {
	if (GetIncrementalReset()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto void clips_set_incremental_reset(bool value)
   Set the incremental reset behavior. Equivalent of
   the CLIPS set-incremental-reset command. */
PHP_FUNCTION(clips_set_incremental_reset) {
	int val;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &val) == FAILURE) {
        RETURN_FALSE;
    }

	SetIncrementalReset(val);
}
/* }}} */



/* {{{ proto bool clips_is_defrule_deletable(string defrule)
   Tell whether or not a defrule can be deleted */
PHP_FUNCTION(clips_is_defrule_deletable) {
	char *dr_name;
	int dr_name_len;
	void *dr_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dr_name, &dr_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dr_ptr = FindDefrule(dr_name);

	if (dr_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defrule '%s'", dr_name);
		RETURN_FALSE;
	}

	if (IsDefruleDeletable(dr_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_undefrule(string defrule)
   Removes a defrule construct from CLIPS */
PHP_FUNCTION(clips_undefrule) {
	char *dr_name;
	int dr_name_len;
	void *dr_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dr_name, &dr_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dr_ptr = FindDefrule(dr_name);

	if (dr_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defrule '%s'", dr_name);
		RETURN_FALSE;
	}

	if (Undefrule(dr_ptr)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Failed to remove defrule '%s'", dr_name);
		RETURN_FALSE;
	}
}
/* }}} */









/* AGENDA FUNCTIONS */



/* {{{ proto void clips_clear_focus_stack()
   Remove all modules from the focus stack. Equivalent of the CLIPS clear-focus-stack command. */
PHP_FUNCTION(clips_clear_focus_stack) {
	ClearFocusStack();
}
/* }}} */



/* {{{ proto bool clips_get_agenda_changed()
   Tell whether or not there have been any changes to the agenda of rule activations. */
PHP_FUNCTION(clips_get_agenda_changed) {
	if (GetAgendaChanged()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto void clips_set_agenda_changed(bool value)
   Set the internal boolean flag indicating that the agenda of rule activations has
   been changed. */
PHP_FUNCTION(clips_set_agenda_changed) {
	int value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
        RETURN_FALSE;
    }

	SetAgendaChanged(value);
}
/* }}} */



/* {{{ proto string clips_get_focus()
   Get the name of the module associated with the current focus. */
PHP_FUNCTION(clips_get_focus) {
	void *module_ptr;

	module_ptr = GetFocus();

	RETURN_STRING(GetDefmoduleName(module_ptr), 1);
}
/* }}} */



/* {{{ proto array clips_get_focus_stack()
   Get an array of module names in the focus stack. */
PHP_FUNCTION(clips_get_focus_stack) {
	DATA_OBJECT retval;
	GetFocusStack(&retval);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto int clips_get_salience_evaluation()
   Get the current value of the salience evaluation behavior. Equivalent of the 
   CLIPS get-salience-evaluation command. */
PHP_FUNCTION(clips_get_salience_evaluation) {
	RETURN_LONG(GetSalienceEvaluation());
}
/* }}} */



/* {{{ proto void clips_set_salience_evaluation(int value)
   Set the current value of the salience evaluation behavior. Equivalent of the 
   CLIPS set-salience-evaluation command. */
PHP_FUNCTION(clips_set_salience_evaluation) {
	int value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
        RETURN_FALSE;
    }

	SetSalienceEvaluation(value);
}
/* }}} */



/* {{{ proto void clips_set_strategy(int value);
   Set the conflict resolution strategy */
PHP_FUNCTION(clips_set_strategy) {
	int strategy;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &strategy) == FAILURE) {
        RETURN_FALSE;
    }

	SetStrategy(strategy);
}
/* }}} */



/* {{{ proto int clips_get_strategy()
   Get the current conflict resolution strategy. Equivalent of the 
   CLIPS get-strategy command. */
PHP_FUNCTION(clips_get_strategy) {
	RETURN_LONG(GetStrategy());
}
/* }}} */



/* {{{ proto string clips_pop_focus()
   Remove the current focus from the focus stack and return the module name
   associated with that focus. Equivalent to the CLIPS pop-focus function. */
PHP_FUNCTION(clips_pop_focus) {
	void *module;	
	module = PopFocus();
	RETURN_STRING(GetDefmoduleName(module), 1);
}
/* }}} */



/* {{{ proto bool clips_focus(string defmodule)
   Sets the current focus */
PHP_FUNCTION(clips_focus) {
	char *module;
	int module_len;
	void *module_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	module_ptr = FindDefmodule(module);

	if (module_ptr == NULL) {
		zend_error(E_WARNING, "Unknown module '%s'", module);
		RETURN_FALSE;
	}

	Focus(module_ptr);
	RETURN_TRUE;
}
/* }}} */



/* {{{ proto void clips_refresh_agenda(string defmodule)
   Recomputes the salience values for all activations on the agenda
   and then reorders the agenda */
PHP_FUNCTION(clips_refresh_agenda) {
	char *module;
	int module_len;
	void *module_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	module_ptr = FindDefmodule(module);

	if (module_ptr == NULL) {
		zend_error(E_WARNING, "Unknown module '%s'", module);
		RETURN_FALSE;
	}

	RefreshAgenda(module_ptr);
}
/* }}} */



/* {{{ proto void clips_reorder_agenda(string defmodule)
   Reorders the agenda based on the current conflict resolution
   strategy and current activation saliences */
PHP_FUNCTION(clips_reorder_agenda) {
	char *module;
	int module_len;
	void *module_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	module_ptr = FindDefmodule(module);

	if (module_ptr == NULL) {
		zend_error(E_WARNING, "Unknown module '%s'", module);
		RETURN_FALSE;
	}

	ReorderAgenda(module_ptr);
}
/* }}} */









/* DEFGLOBAL FUNCTIONS */



/* {{{ proto string clips_defglobal_module(string defglobal_name)
   Get the name of the module in which a defglobal is defined */
PHP_FUNCTION(clips_defglobal_module) {
	char *dg_name;
	int dg_name_len;
	void *dg_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dg_name, &dg_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dg_ptr = FindDefglobal(dg_name);

	if (dg_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defglobal '%s'", dg_name);
		RETURN_FALSE;
	}

	RETURN_STRING(DefglobalModule(dg_ptr), 1);
}
/* }}} */



/* {{{ proto mixed clips_get_defglobal_value(string defglobal_name)
   Retrieve the value of a defglobal. */
PHP_FUNCTION(clips_get_defglobal_value) {
	char *dg_name;
	int dg_name_len;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dg_name, &dg_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	GetDefglobalValue(dg_name, &retval);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto bool clips_get_globals_changed()
   Tell whether or not any changes to global variables have occurred. */
PHP_FUNCTION(clips_get_globals_changed) {
	if (GetGlobalsChanged()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_get_reset_globals()
   Get the current value of the reset global variables behavior. */
PHP_FUNCTION(clips_get_reset_globals) {
	if (GetResetGlobals()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto string clips_is_defglobal_deletable(string defglobal_name)
   Tell whether or not a particular defglobal can be deleted. */
PHP_FUNCTION(clips_is_defglobal_deletable) {
	char *dg_name;
	int dg_name_len;
	void *dg_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dg_name, &dg_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dg_ptr = FindDefglobal(dg_name);

	if (dg_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defglobal '%s'", dg_name);
		RETURN_FALSE;
	}

	if (IsDefglobalDeletable(dg_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto void clips_set_defglobal_value(string defglobal_name, mixed value)
   Set the value of a defglobal. */
PHP_FUNCTION(clips_set_defglobal_value) {
	char *dg_name;
	int dg_name_len;
	zval *val;
	DATA_OBJECT setval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &dg_name, &dg_name_len, &val) == FAILURE) {
        RETURN_FALSE;
    }

	zval_to_dataobj(val, &setval);

	if (SetDefglobalValue(dg_name, &setval)) {
		RETURN_TRUE;
	} else {
		zend_error(E_WARNING, "Unknown defglobal '%s'", dg_name);
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_undefglobal(string defglobal_name)
   Remove a defglobal from the CLIPS environment. */
PHP_FUNCTION(clips_undefglobal) {
	char *dg_name;
	int dg_name_len;
	void *dg_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dg_name, &dg_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dg_ptr = FindDefglobal(dg_name);

	if (dg_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defglobal '%s'", dg_name);
		RETURN_FALSE;
	}

	if (Undefglobal(dg_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */









/* DEFFUNCTION FUNCTIONS */



/* {{{ proto string clips_deffunction_module(string deffunction_name)
   Get the name of the module in which a deffunction is defined */
PHP_FUNCTION(clips_deffunction_module) {
	char *df_name;
	int df_name_len;
	void *df_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &df_name, &df_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	df_ptr = FindDeffunction(df_name);

	if (df_ptr == NULL) {
		zend_error(E_WARNING, "Undefined deffunction '%s'", df_name);
		RETURN_FALSE;
	}

	RETURN_STRING(DeffunctionModule(df_ptr), 1);
}
/* }}} */



/* {{{ proto array clips_get_deffunction_list([string module])
   Retrieve a list of the current deffunctions in the CLIPS environment. */
PHP_FUNCTION(clips_get_deffunction_list) {
	char *module;
	int module_len;
	void *module_ptr;
	DATA_OBJECT df_list;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (ZEND_NUM_ARGS() > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	} else {
		module_ptr = NULL;
	}

	GetDeffunctionList(&df_list, module_ptr);
	RETURN_DATA_OBJECT(df_list);
}
/* }}} */



/* {{{ proto bool clips_is_deffunction_deletable(string deffunction_name)
   Tell whether or not a particular deffunction can be deleted */
PHP_FUNCTION(clips_is_deffunction_deletable) {
	char *df_name;
	int df_name_len;
	void *df_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &df_name, &df_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	df_ptr = FindDeffunction(df_name);

	if (df_ptr == NULL) {
		zend_error(E_WARNING, "Undefined deffunction '%s'", df_name);
		RETURN_FALSE;
	}

	if (IsDeffunctionDeletable(df_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_undeffunction(string deffunction_name)
   Remove a deffunction from the CLIPS environment. */
PHP_FUNCTION(clips_undeffunction) {
	char *df_name;
	int df_name_len;
	void *df_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &df_name, &df_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	df_ptr = FindDeffunction(df_name);

	if (df_ptr == NULL) {
		zend_error(E_WARNING, "Undefined deffunction '%s'", df_name);
		RETURN_FALSE;
	}

	if (Undeffunction(df_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */









/* DEFGENERIC FUNCTIONS */



/* {{{ proto string clips_defgeneric_module(string defgeneric_name)
   Get the name of the module in which a defgeneric is defined */
PHP_FUNCTION(clips_defgeneric_module) {
	char *dg_name;
	int dg_name_len;
	void *dg_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dg_name, &dg_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dg_ptr = FindDefgeneric(dg_name);

	if (dg_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defgeneric '%s'", dg_name);
		RETURN_FALSE;
	}

	RETURN_STRING(DefgenericModule(dg_ptr), 1);
}
/* }}} */



/* {{{ proto array clips_get_defgeneric_list([string module])
   Retrieve a list of the current defgenerics in the CLIPS environment. */
PHP_FUNCTION(clips_get_defgeneric_list) {
	char *module;
	int module_len;
	void *module_ptr;
	DATA_OBJECT dg_list;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (ZEND_NUM_ARGS() > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	} else {
		module_ptr = NULL;
	}

	GetDefgenericList(&dg_list, module_ptr);
	RETURN_DATA_OBJECT(dg_list);
}
/* }}} */



/* {{{ proto bool clips_is_defgeneric_deletable(string defgeneric_name)
   Tell whether or not a particular defgeneric can be deleted */
PHP_FUNCTION(clips_is_defgeneric_deletable) {
	char *dg_name;
	int dg_name_len;
	void *dg_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dg_name, &dg_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dg_ptr = FindDefgeneric(dg_name);

	if (dg_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defgeneric '%s'", dg_name);
		RETURN_FALSE;
	}

	if (IsDefgenericDeletable(dg_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_undefgeneric(string defgeneric_name)
   Remove a defgeneric from the CLIPS environment. */
PHP_FUNCTION(clips_undefgeneric) {
	char *dg_name;
	int dg_name_len;
	void *dg_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &dg_name, &dg_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	dg_ptr = FindDefgeneric(dg_name);

	if (dg_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defgeneric '%s'", dg_name);
		RETURN_FALSE;
	}

	if (Undefgeneric(dg_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */









/* DEFCLASS FUNCTIONS */



/* {{{ proto bool clips_class_abstract(string defclass)
   Tell whether or not a class is abstract. Equivalent to the CLIPS class-abstractp command. */
PHP_FUNCTION(clips_class_abstract) {
	char *class_name;
	int class_name_len;
	void *class_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defclass '%s'", class_name);
		RETURN_FALSE;
	}

	if (ClassAbstractP(class_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_class_reactive(string defclass)
   Tell whether or not a class is reactive. Equivalent to the CLIPS class-reactivep command. */
PHP_FUNCTION(clips_class_reactive) {
	char *class_name;
	int class_name_len;
	void *class_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defclass '%s'", class_name);
		RETURN_FALSE;
	}

	if (ClassReactiveP(class_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto array clips_class_slots(string defclass [, bool inherit])
   Groups the names of the slots of a class into an array */
PHP_FUNCTION(clips_class_slots) {
	void *class_ptr;
	char *class_name;
	int class_name_len, inherit = 1;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &class_name, &class_name_len, &inherit) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defclass '%s'", class_name);
		RETURN_FALSE;
	}

	ClassSlots(class_ptr, &retval, inherit);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto array clips_class_subclasses(resource defclass [, bool inherit])
   Groups the names of subclasses of a class into an array */
PHP_FUNCTION(clips_class_subclasses) {
	void *class_ptr;
	char *class_name;
	int class_name_len, inherit = 1;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &class_name, &class_name_len, &inherit) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defclass '%s'", class_name);
		RETURN_FALSE;
	}

	ClassSubclasses(class_ptr, &retval, inherit);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto array clips_class_superclasses(resource defclass [, bool inherit])
   Groups the names of parent classes of a class into an array */
PHP_FUNCTION(clips_class_superclasses) {
	void *class_ptr;
	char *class_name;
	int class_name_len, inherit = 1;
	DATA_OBJECT retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &class_name, &class_name_len, &inherit) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defclass '%s'", class_name);
		RETURN_FALSE;
	}

	ClassSuperclasses(class_ptr, &retval, inherit);
	RETURN_DATA_OBJECT(retval);
}
/* }}} */



/* {{{ proto string clips_defclass_module(string defclass)
   Get the name of the module in which a defclass is defined */
PHP_FUNCTION(clips_defclass_module) {
	char *class_name;
	int class_name_len;
	void *class_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defclass '%s'", class_name);
		RETURN_FALSE;
	}

	RETURN_STRING(DefclassModule(class_ptr), 1);
}
/* }}} */



/* {{{ proto int clips_get_class_defaults_mode()
   Get the current class defaults mode. */
PHP_FUNCTION(clips_get_class_defaults_mode) {
	RETURN_LONG(GetClassDefaultsMode());
}
/* }}} */



/* {{{ proto void clips_set_class_defaults_mode(int mode)
   Set the current class defaults mode. */
PHP_FUNCTION(clips_set_class_defaults_mode) {
	unsigned short mode;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &mode) == FAILURE) {
        RETURN_FALSE;
    }

	SetClassDefaultsMode(mode);
}
/* }}} */



/* {{{ proto array clips_get_defclass_list([string module])
   Retrieve a list of the current defclasss in the CLIPS environment. */
PHP_FUNCTION(clips_get_defclass_list) {
	char *module;
	int module_len;
	void *module_ptr;
	DATA_OBJECT class_list;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	if (ZEND_NUM_ARGS() > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	} else {
		module_ptr = NULL;
	}

	GetDefclassList(&class_list, module_ptr);
	RETURN_DATA_OBJECT(class_list);
}
/* }}} */



/* {{{ proto bool clips_is_defclass_deletable(string defclass)
   Tell whether or not a particular class and all its subclasses can be deleted */
PHP_FUNCTION(clips_is_defclass_deletable) {
	char *class_name;
	int class_name_len;
	void *class_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defclass '%s'", class_name);
		RETURN_FALSE;
	}

	if (IsDefclassDeletable(class_name)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_subclass(string defclass1, string defclass2)
   Returns TRUE if the first defclass is a subclass of the second defclass, FALSE otherwise. */
PHP_FUNCTION(clips_subclass) {
	char *c1, *c2;
	int c1_len, c2_len;
	void *c1_ptr, *c2_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &c1, &c1_len, &c2, &c2_len) == FAILURE) {
        RETURN_FALSE;
    }

	c1_ptr = FindDefclass(c1);
	c2_ptr = FindDefclass(c2);

	if (c1_ptr == NULL || c2_ptr == NULL) {
		if (c1_ptr == NULL) {
			zend_error(E_WARNING, "Unknown defclass '%s'", c1);
		}

		if (c2_ptr == NULL) {
			zend_error(E_WARNING, "Unknown defclass '%s'", c2);
		}

		RETURN_FALSE;
	}

	if (SubclassP(c1_ptr, c2_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_superclass(string defclass1, string defclass2)
   Returns TRUE if the first defclass is a parent to the second defclass, FALSE otherwise. */
PHP_FUNCTION(clips_superclass) {
	char *c1, *c2;
	int c1_len, c2_len;
	void *c1_ptr, *c2_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &c1, &c1_len, &c2, &c2_len) == FAILURE) {
        RETURN_FALSE;
    }

	c1_ptr = FindDefclass(c1);
	c2_ptr = FindDefclass(c2);

	if (c1_ptr == NULL || c2_ptr == NULL) {
		if (c1_ptr == NULL) {
			zend_error(E_WARNING, "Unknown defclass '%s'", c1);
		}

		if (c2_ptr == NULL) {
			zend_error(E_WARNING, "Unknown defclass '%s'", c2);
		}

		RETURN_FALSE;
	}

	if (SuperclassP(c1_ptr, c2_ptr)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto bool clips_undefclass(string defclass)
   Remove a class and all its subclasses from the CLIPS environment */
PHP_FUNCTION(clips_undefclass) {
	char *class_name;
	int class_name_len;
	void *class_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class_name, &class_name_len) == FAILURE) {
        RETURN_FALSE;
    }

	class_ptr = FindDefclass(class_name);

	if (class_ptr == NULL) {
		zend_error(E_WARNING, "Undefined defclass '%s'", class_name);
		RETURN_FALSE;
	}

	if (Undefclass(class_name)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */









/* INSTANCE FUNCTIONS */



/* {{{ proto bool clips_get_instances_changed()
   Determine whether or not any changes to instances of user-defined instances 
   have occurred. */
PHP_FUNCTION(clips_get_instances_changed) {
	if (GetInstancesChanged()) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */



/* {{{ proto void clips_set_instances_changed(bool value)
   Set the internal boolean flag which indicates when changes to instances
   of user-defined classes have occurred. */
PHP_FUNCTION(clips_set_instances_changed) {
	int value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &value) == FAILURE) {
        RETURN_FALSE;
    }

	SetInstancesChanged(value);
}
/* }}} */



/* {{{ proto array clips_get_instance_list(string class [, bool include_subclasses]);
   Retrieves a list of instances in the CLIPS environment of the given defclass. */
PHP_FUNCTION(clips_get_instance_list) {
	void *multifield_ptr, *instance_ptr, *defclass_ptr;
	int i, end, defclass_name_len, include_subclasses = 0;
	zval *cur_instance;
	char *defclass_name;
	DATA_OBJECT slot_names, slot_value, iteration_data, subclass_search;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &defclass_name, &defclass_name_len, include_subclasses) == FAILURE) {
        RETURN_FALSE;
    }

	/* Initialize the return value to an empty array */
	array_init(return_value);

	/* Get a pointer to the defclass that was specified */
	defclass_ptr = FindDefclass(defclass_name);

	if (defclass_ptr == NULL) {
		zend_error(E_WARNING, "Defclass '%s' is not defined", defclass_name);
		RETURN_FALSE;
	}

	/* Get a pointer to the first instance of the specified class */
	if (include_subclasses) {
		ClassSubclasses(defclass_ptr, &subclass_search, 0);

		/* check if the defclass actually has subclasses
		   otherwise if we call GetNextInstanceInClassAndSubclasses, it will produce a segfault */
		if (GetDOEnd(subclass_search) > 0) {
			instance_ptr = GetNextInstanceInClassAndSubclasses(defclass_ptr, NULL, &iteration_data);
		} else {
			instance_ptr = GetNextInstanceInClass(defclass_ptr, NULL);
		}
	} else {
		instance_ptr = GetNextInstanceInClass(defclass_ptr, NULL);
	}

	/* Get the slot names of the defclass */
	ClassSlots(defclass_ptr, &slot_names, 1);

	/* Iterate through the list of instance pointers */
	while (instance_ptr != NULL) {
		
		/* Initialize an array to contain an array representing the current instance */
		MAKE_STD_ZVAL(cur_instance);
		array_init(cur_instance);
		
		end = GetDOEnd(slot_names);
		multifield_ptr = GetValue(slot_names);


		/* Iterate through the instance slots and retrieve the values in each slot */
		for (i = GetDOBegin(slot_names); i <= end; i++) {
			
			/* Store the current slot value into a new DATA_OBJECT */
			DirectGetSlot(instance_ptr, ValueToString(GetMFValue(multifield_ptr, i)), &slot_value);
			
			/* Convert the DATA_OBJECT into a PHP data type and append to the instance array */
			switch (GetType(slot_value)) {
				case FLOAT: 
					add_assoc_double(cur_instance, ValueToString(GetMFValue(multifield_ptr, i)), DOToDouble(slot_value));
					break;
				
				case STRING: case SYMBOL: case INSTANCE_NAME: 
					add_assoc_string(cur_instance, ValueToString(GetMFValue(multifield_ptr, i)), DOToString(slot_value), 1);
					break;
				
				case INTEGER: 
					add_assoc_long(cur_instance, ValueToString(GetMFValue(multifield_ptr, i)), DOToLong(slot_value));
					break;
			}
		}


		/* Add the instance array to the return value array */
		add_next_index_zval(return_value, cur_instance);

		/* Get a pointer to the first instance of the specified class */
		if (include_subclasses) {
			ClassSubclasses(defclass_ptr, &subclass_search, 0);

			/* check if the defclass actually has subclasses
			   otherwise if we call GetNextInstanceInClassAndSubclasses, it will produce a segfault */
			if (GetDOEnd(subclass_search) > 0) {
				instance_ptr = GetNextInstanceInClassAndSubclasses(defclass_ptr, instance_ptr, &iteration_data);
			} else {
				instance_ptr = GetNextInstanceInClass(defclass_ptr, instance_ptr);
			}
		} else {
			instance_ptr = GetNextInstanceInClass(defclass_ptr, instance_ptr);
		}

	} /* End while */
}
/* }}} */



/* {{{ proto bool clips_create_instance(mixed values, string defclass, string name [, string module]) 
   Create a new instance with the specified name of the specified class. */
PHP_FUNCTION(clips_create_instance) {
	char *defclass, *name, *module, *key;
	int defclass_len, name_len, module_len = 0, errors = 0;
	uint key_type, key_len;
	ulong idx;
	zval *values, **val;
	void *defclass_ptr, *ins_ptr, *module_ptr, *prev_module_ptr;
	DATA_OBJECT slot_value;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ass|s", &values, &defclass, &defclass_len, &name, &name_len, &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	defclass_ptr = FindDefclass(defclass);

	if (defclass_ptr == NULL) {
		zend_error(E_WARNING, "Unknown defclass '%s'", defclass);
		RETURN_FALSE;
	}

	/* switch module if necessary */
	if (module_len > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
	
		prev_module_ptr = GetCurrentModule();

		if (module_ptr != prev_module_ptr) {
			SetCurrentModule(module_ptr);
		}
	}

	ins_ptr = CreateRawInstance(defclass_ptr, name);

	/* switch back to previous module if necessary */
	if (module_len > 0 && module_ptr != prev_module_ptr) {
		SetCurrentModule(prev_module_ptr);
	}

	if (ins_ptr == NULL) {
		zend_error(E_WARNING, "Failed to create instance '%s'", name);
		RETURN_FALSE;
	}

	zend_hash_internal_pointer_reset(Z_ARRVAL_P(values));
			
	while (zend_hash_get_current_data(Z_ARRVAL_P(values), (void **)&val) == SUCCESS) {
		/* get key */
		key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(values), &key, &key_len, &idx, 0, NULL);
		
		/* get value */
		SEPARATE_ZVAL(val);

		zval_to_dataobj(*val, &slot_value);

		/* save the current slot value */
		if ( ! DirectPutSlot(ins_ptr, key, &slot_value)) {
			zend_error(E_WARNING, "Failed to save value to instance slot '%s', the slot may not exist or the value is the wrong type", key);
			errors++;
		}

		zend_hash_move_forward(Z_ARRVAL_P(values));
	}

	if (errors) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */



/* {{{ proto bool clips_make_instance(string make_command [, string module]) 
   Create and initialize an instance of a class from a string. */
PHP_FUNCTION(clips_make_instance) {
	char *cmd, *module;
	int cmd_len, module_len = 0;
	void *ins_ptr, *module_ptr, *prev_module_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &cmd, &cmd_len, &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	/* switch module if necessary */
	if (module_len > 0) {
		module_ptr = FindDefmodule(module);

		if (module_ptr == NULL) {
			zend_error(E_WARNING, "Unknown module '%s'", module);
			RETURN_FALSE;
		}
		
		prev_module_ptr = GetCurrentModule();

		if (module_ptr != prev_module_ptr) {
			SetCurrentModule(module_ptr);
		}
	}

	ins_ptr = MakeInstance(cmd);

	/* switch back to previous module if necessary */
	if (module_len > 0 && module_ptr != prev_module_ptr) {
		SetCurrentModule(prev_module_ptr);
	}

	if (ins_ptr == NULL) {
		zend_error(E_WARNING, "Failed to create instance from string");
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */









/* DEFMODULE FUNCTIONS */



/* {{{ proto array clips_get_defmodule_list()
   Retrieve a list of defmodules in the CLIPS environemnt */
PHP_FUNCTION(clips_get_defmodule_list) {
	DATA_OBJECT module_list;
	GetDefmoduleList(&module_list);
	RETURN_DATA_OBJECT(module_list);
}
/* }}} */



/* {{{ proto void clips_set_current_module(string module)
   Set the current module */
PHP_FUNCTION(clips_set_current_module) {
	char *module;
	int module_len;
	void *module_ptr;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &module, &module_len) == FAILURE) {
        RETURN_FALSE;
    }

	module_ptr = FindDefmodule(module);

	if (module_ptr == NULL) {
		zend_error(E_WARNING, "Unknown module '%s'", module);
		RETURN_FALSE;
	}

	SetCurrentModule(module);
}
/* }}} */


