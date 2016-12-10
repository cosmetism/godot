/*************************************************************************/
/*  script_text_editor.cpp                                               */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "script_text_editor.h"
#include "tools/editor/editor_settings.h"
#include "os/keyboard.h"
#include "tools/editor/script_editor_debugger.h"
#include "tools/editor/editor_node.h"

Vector<String> ScriptTextEditor::get_functions()  {


	String errortxt;
	int line=-1,col;
	TextEdit *te=code_editor->get_text_edit();
	String text = te->get_text();
	List<String> fnc;

	if (script->get_language()->validate(text,line,col,errortxt,script->get_path(),&fnc)) {

		//if valid rewrite functions to latest
		functions.clear();
		for (List<String>::Element *E=fnc.front();E;E=E->next()) {

			functions.push_back(E->get());
		}


	}

	return functions;
}

void ScriptTextEditor::apply_code() {

	if (script.is_null())
		return;
//	print_line("applying code");
	script->set_source_code(code_editor->get_text_edit()->get_text());
	script->update_exports();
}

Ref<Script> ScriptTextEditor::get_edited_script() const {

	return script;
}

bool ScriptTextEditor::goto_method(const String& p_method) {


	Vector<String> functions = get_functions();

	String method_search = p_method + ":";

	for (int i=0;i<functions.size();i++) {
		String function=functions[i];

		if (function.begins_with(method_search)) {

			int line=function.get_slice(":",1).to_int();
			goto_line(line-1);
			return true;
		}
	}

	return false;
}

void ScriptTextEditor::_load_theme_settings() {

	TextEdit *text_edit = code_editor->get_text_edit();

	text_edit->clear_colors();

	/* keyword color */


	text_edit->add_color_override("background_color", EDITOR_DEF("text_editor/background_color",Color(0,0,0,0)));
	text_edit->add_color_override("completion_background_color", EDITOR_DEF("text_editor/completion_background_color", Color(0,0,0,0)));
	text_edit->add_color_override("completion_selected_color", EDITOR_DEF("text_editor/completion_selected_color", Color::html("434244")));
	text_edit->add_color_override("completion_existing_color", EDITOR_DEF("text_editor/completion_existing_color", Color::html("21dfdfdf")));
	text_edit->add_color_override("completion_scroll_color", EDITOR_DEF("text_editor/completion_scroll_color", Color::html("ffffff")));
	text_edit->add_color_override("completion_font_color", EDITOR_DEF("text_editor/completion_font_color", Color::html("aaaaaa")));
	text_edit->add_color_override("font_color",EDITOR_DEF("text_editor/text_color",Color(0,0,0)));
	text_edit->add_color_override("line_number_color",EDITOR_DEF("text_editor/line_number_color",Color(0,0,0)));
	text_edit->add_color_override("caret_color",EDITOR_DEF("text_editor/caret_color",Color(0,0,0)));
	text_edit->add_color_override("caret_background_color",EDITOR_DEF("text_editor/caret_background_color",Color(0,0,0)));
	text_edit->add_color_override("font_selected_color",EDITOR_DEF("text_editor/text_selected_color",Color(1,1,1)));
	text_edit->add_color_override("selection_color",EDITOR_DEF("text_editor/selection_color",Color(0.2,0.2,1)));
	text_edit->add_color_override("brace_mismatch_color",EDITOR_DEF("text_editor/brace_mismatch_color",Color(1,0.2,0.2)));
	text_edit->add_color_override("current_line_color",EDITOR_DEF("text_editor/current_line_color",Color(0.3,0.5,0.8,0.15)));
	text_edit->add_color_override("word_highlighted_color",EDITOR_DEF("text_editor/word_highlighted_color",Color(0.8,0.9,0.9,0.15)));
	text_edit->add_color_override("number_color",EDITOR_DEF("text_editor/number_color",Color(0.9,0.6,0.0,2)));
	text_edit->add_color_override("function_color",EDITOR_DEF("text_editor/function_color",Color(0.4,0.6,0.8)));
	text_edit->add_color_override("member_variable_color",EDITOR_DEF("text_editor/member_variable_color",Color(0.9,0.3,0.3)));
	text_edit->add_color_override("mark_color", EDITOR_DEF("text_editor/mark_color", Color(1.0,0.4,0.4,0.4)));
	text_edit->add_color_override("breakpoint_color", EDITOR_DEF("text_editor/breakpoint_color", Color(0.8,0.8,0.4,0.2)));
	text_edit->add_color_override("search_result_color",EDITOR_DEF("text_editor/search_result_color",Color(0.05,0.25,0.05,1)));
	text_edit->add_color_override("search_result_border_color",EDITOR_DEF("text_editor/search_result_border_color",Color(0.1,0.45,0.1,1)));
	text_edit->add_color_override("symbol_color",EDITOR_DEF("text_editor/symbol_color",Color::hex(0x005291ff)));
	text_edit->add_constant_override("line_spacing", EDITOR_DEF("text_editor/line_spacing",4));

	Color keyword_color= EDITOR_DEF("text_editor/keyword_color",Color(0.5,0.0,0.2));

	List<String> keywords;
	script->get_language()->get_reserved_words(&keywords);
	for(List<String>::Element *E=keywords.front();E;E=E->next()) {

		text_edit->add_keyword_color(E->get(),keyword_color);
	}

	//colorize core types
	Color basetype_color= EDITOR_DEF("text_editor/base_type_color",Color(0.3,0.3,0.0));

	text_edit->add_keyword_color("Vector2",basetype_color);
	text_edit->add_keyword_color("Vector3",basetype_color);
	text_edit->add_keyword_color("Plane",basetype_color);
	text_edit->add_keyword_color("Quat",basetype_color);
	text_edit->add_keyword_color("AABB",basetype_color);
	text_edit->add_keyword_color("Matrix3",basetype_color);
	text_edit->add_keyword_color("Transform",basetype_color);
	text_edit->add_keyword_color("Color",basetype_color);
	text_edit->add_keyword_color("Image",basetype_color);
	text_edit->add_keyword_color("InputEvent",basetype_color);
	text_edit->add_keyword_color("Rect2",basetype_color);
	text_edit->add_keyword_color("NodePath",basetype_color);

	//colorize engine types
	Color type_color= EDITOR_DEF("text_editor/engine_type_color",Color(0.0,0.2,0.4));

	List<StringName> types;
	ObjectTypeDB::get_type_list(&types);

	for(List<StringName>::Element *E=types.front();E;E=E->next()) {

		String n = E->get();
		if (n.begins_with("_"))
			n = n.substr(1, n.length());

		text_edit->add_keyword_color(n,type_color);
	}

	//colorize comments
	Color comment_color = EDITOR_DEF("text_editor/comment_color",Color::hex(0x797e7eff));
	List<String> comments;
	script->get_language()->get_comment_delimiters(&comments);

	for(List<String>::Element *E=comments.front();E;E=E->next()) {

		String comment = E->get();
		String beg = comment.get_slice(" ",0);
		String end = comment.get_slice_count(" ")>1?comment.get_slice(" ",1):String();

		text_edit->add_color_region(beg,end,comment_color,end=="");
	}

	//colorize strings
	Color string_color = EDITOR_DEF("text_editor/string_color",Color::hex(0x6b6f00ff));
	List<String> strings;
	script->get_language()->get_string_delimiters(&strings);

	for (List<String>::Element *E=strings.front();E;E=E->next()) {

		String string = E->get();
		String beg = string.get_slice(" ",0);
		String end = string.get_slice_count(" ")>1?string.get_slice(" ",1):String();
		text_edit->add_color_region(beg,end,string_color,end=="");
	}
}


void ScriptTextEditor::reload_text() {

	ERR_FAIL_COND(script.is_null())	;

	TextEdit *te = code_editor->get_text_edit();
	int column = te->cursor_get_column();
	int row = te->cursor_get_line();
	int h = te->get_h_scroll();
	int v = te->get_v_scroll();

	te->set_text(script->get_source_code());
	te->clear_undo_history();
	te->cursor_set_line(row);
	te->cursor_set_column(column);
	te->set_h_scroll(h);
	te->set_v_scroll(v);

	te->tag_saved_version();

	code_editor->update_line_and_column();

}

void ScriptTextEditor::_notification(int p_what) {

	if (p_what==NOTIFICATION_READY) {

		//emit_signal("name_changed");
	}
}

void ScriptTextEditor::add_callback(const String& p_function,StringArray p_args) {

	String code = code_editor->get_text_edit()->get_text();
	int pos = script->get_language()->find_function(p_function,code);
	if (pos==-1) {
		//does not exist
		code_editor->get_text_edit()->deselect();
		pos=code_editor->get_text_edit()->get_line_count()+2;
		String func = script->get_language()->make_function("",p_function,p_args);
		//code=code+func;
		code_editor->get_text_edit()->cursor_set_line(pos+1);
		code_editor->get_text_edit()->cursor_set_column(1000000); //none shall be that big
		code_editor->get_text_edit()->insert_text_at_cursor("\n\n"+func);
	}
	code_editor->get_text_edit()->cursor_set_line(pos);
	code_editor->get_text_edit()->cursor_set_column(1);
}

void ScriptTextEditor::update_settings() {

	code_editor->update_editor_settings();
}

bool ScriptTextEditor::is_unsaved()  {

	return code_editor->get_text_edit()->get_version()!=code_editor->get_text_edit()->get_saved_version();
}

Variant ScriptTextEditor::get_edit_state() {

	Dictionary state;

	state["scroll_pos"]=code_editor->get_text_edit()->get_v_scroll();
	state["column"]=code_editor->get_text_edit()->cursor_get_column();
	state["row"]=code_editor->get_text_edit()->cursor_get_line();

	return state;
}

void ScriptTextEditor::trim_trailing_whitespace() {

	TextEdit *tx = code_editor->get_text_edit();

	bool trimed_whitespace = false;
	for (int i = 0; i < tx->get_line_count(); i++) {
		String line = tx->get_line(i);
		if (line.ends_with(" ") || line.ends_with("\t")) {

			if (!trimed_whitespace) {
				tx->begin_complex_operation();
				trimed_whitespace = true;
			}

			int end = 0;
			for (int j = line.length() - 1; j > -1; j--) {
				if (line[j] != ' ' && line[j] != '\t') {
					end = j+1;
					break;
				}
			}
			tx->set_line(i, line.substr(0, end));
		}
	}
	if (trimed_whitespace) {
		tx->end_complex_operation();
		tx->update();
	}
}

void ScriptTextEditor::tag_saved_version() {

	code_editor->get_text_edit()->tag_saved_version();
}

void ScriptTextEditor::goto_line(int p_line, bool p_with_error) {
	code_editor->get_text_edit()->cursor_set_line(p_line);
}

void ScriptTextEditor::ensure_focus() {

	code_editor->get_text_edit()->grab_focus();
}

void ScriptTextEditor::set_edit_state(const Variant& p_state) {

	Dictionary state=p_state;
	code_editor->get_text_edit()->set_v_scroll(state["scroll_pos"]);
	code_editor->get_text_edit()->cursor_set_column( state["column"]);
	code_editor->get_text_edit()->cursor_set_line( state["row"] );
	code_editor->get_text_edit()->grab_focus();

	//int scroll_pos;
	//int cursor_column;
	//int cursor_row;
}

String ScriptTextEditor::get_name()  {
	String name;

	if (script->get_path().find("local://")==-1 && script->get_path().find("::")==-1) {
		name=script->get_path().get_file();
		if (is_unsaved()) {
			name+="(*)";
		}
	} else if (script->get_name()!="")
		name=script->get_name();
	else
		name=script->get_type()+"("+itos(script->get_instance_ID())+")";

	return name;

}

Ref<Texture> ScriptTextEditor::get_icon() {

	if (get_parent_control() && get_parent_control()->has_icon(script->get_type(),"EditorIcons")) {
		return get_parent_control()->get_icon(script->get_type(),"EditorIcons");
	}

	return Ref<Texture>();
}



void ScriptTextEditor::set_edited_script(const Ref<Script>& p_script) {

	ERR_FAIL_COND(!script.is_null());

	script=p_script;


	_load_theme_settings();

	code_editor->get_text_edit()->set_text(script->get_source_code());
	code_editor->get_text_edit()->clear_undo_history();
	code_editor->get_text_edit()->tag_saved_version();

	emit_signal("name_changed");
	code_editor->update_line_and_column();
}


void ScriptTextEditor::_validate_script() {

	String errortxt;
	int line=-1,col;
	TextEdit *te=code_editor->get_text_edit();

	String text = te->get_text();
	List<String> fnc;

	if (!script->get_language()->validate(text,line,col,errortxt,script->get_path(),&fnc)) {
		String error_text="error("+itos(line)+","+itos(col)+"): "+errortxt;
		code_editor->set_error(error_text);
	} else {
		code_editor->set_error("");
		line=-1;
		if (!script->is_tool()) {
			script->set_source_code(text);
			script->update_exports();
			//script->reload(); //will update all the variables in property editors
		}

		functions.clear();
		for (List<String>::Element *E=fnc.front();E;E=E->next()) {

			functions.push_back(E->get());
		}

	}

	line--;
	for(int i=0;i<te->get_line_count();i++) {
		te->set_line_as_marked(i,line==i);
	}

	emit_signal("name_changed");
}


static Node* _find_node_for_script(Node* p_base, Node*p_current, const Ref<Script>& p_script) {

	if (p_current->get_owner()!=p_base && p_base!=p_current)
		return NULL;
	Ref<Script> c = p_current->get_script();
	if (c==p_script)
		return p_current;
	for(int i=0;i<p_current->get_child_count();i++) {
		Node *found = _find_node_for_script(p_base,p_current->get_child(i),p_script);
		if (found)
			return found;
	}

	return NULL;
}

static void _find_changed_scripts_for_external_editor(Node* p_base, Node*p_current, Set<Ref<Script> > &r_scripts) {

	if (p_current->get_owner()!=p_base && p_base!=p_current)
		return;
	Ref<Script> c = p_current->get_script();

	if (c.is_valid())
		r_scripts.insert(c);

	for(int i=0;i<p_current->get_child_count();i++) {
		_find_changed_scripts_for_external_editor(p_base,p_current->get_child(i),r_scripts);
	}

}

void ScriptEditor::_update_modified_scripts_for_external_editor(Ref<Script> p_for_script) {

	if (!bool(EditorSettings::get_singleton()->get("external_editor/use_external_editor")))
		return;

	Set<Ref<Script> > scripts;

	Node *base = get_tree()->get_edited_scene_root();
	if (base) {
		_find_changed_scripts_for_external_editor(base,base,scripts);
	}

	for (Set<Ref<Script> >::Element *E=scripts.front();E;E=E->next()) {

		Ref<Script> script = E->get();

		if (p_for_script.is_valid() && p_for_script!=script)
			continue;

		if (script->get_path()=="" || script->get_path().find("local://")!=-1 || script->get_path().find("::")!=-1) {

			continue; //internal script, who cares, though weird
		}

		uint64_t last_date = script->get_last_modified_time();
		uint64_t date = FileAccess::get_modified_time(script->get_path());

		if (last_date!=date) {

			Ref<Script> rel_script = ResourceLoader::load(script->get_path(),script->get_type(),true);
			ERR_CONTINUE(!rel_script.is_valid());
			script->set_source_code( rel_script->get_source_code() );
			script->set_last_modified_time( rel_script->get_last_modified_time() );
			script->update_exports();
		}

	}
}


void ScriptTextEditor::_code_complete_scripts(void* p_ud,const String& p_code, List<String>* r_options) {

	ScriptTextEditor *ste = (ScriptTextEditor *)p_ud;
	ste->_code_complete_script(p_code,r_options);
}

void ScriptTextEditor::_code_complete_script(const String& p_code, List<String>* r_options) {

	if (color_panel->is_visible()) return;
	Node *base = get_tree()->get_edited_scene_root();
	if (base) {
		base = _find_node_for_script(base,base,script);
	}
	String hint;
	Error err = script->get_language()->complete_code(p_code,script->get_path().get_base_dir(),base,r_options,hint);
	if (hint!="") {
		code_editor->get_text_edit()->set_code_hint(hint);
	}

}

void ScriptTextEditor::_breakpoint_toggled(int p_row) {

	ScriptEditor::get_singleton()->get_debugger()->set_breakpoint(script->get_path(),p_row+1,code_editor->get_text_edit()->is_line_set_as_breakpoint(p_row));

}

static void swap_lines(TextEdit *tx, int line1, int line2)
{
    String tmp = tx->get_line(line1);
    String tmp2 = tx->get_line(line2);
    tx->set_line(line2, tmp);
    tx->set_line(line1, tmp2);

    tx->cursor_set_line(line2);
}

void ScriptTextEditor::_lookup_symbol(const String& p_symbol,int p_row, int p_column) {

	Node *base = get_tree()->get_edited_scene_root();
	if (base) {
		base = _find_node_for_script(base,base,script);
	}


	ScriptLanguage::LookupResult result;
	if (script->get_language()->lookup_code(code_editor->get_text_edit()->get_text_for_lookup_completion(),p_symbol,script->get_path().get_base_dir(),base,result)==OK) {

		_goto_line(p_row);

		switch(result.type) {
			case ScriptLanguage::LookupResult::RESULT_SCRIPT_LOCATION: {

				if (result.script.is_valid()) {
					emit_signal("request_open_script_at_line",result.script,result.location-1);
				} else {
					emit_signal("request_save_history");
					_goto_line(result.location-1);
				}
			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS: {
				emit_signal("go_to_help","class_name:"+result.class_name);
			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_CONSTANT: {

				StringName cname = result.class_name;
				bool success;
				while(true) {
					ObjectTypeDB::get_integer_constant(cname,result.class_member,&success);
					if (success) {
						result.class_name=cname;
						cname=ObjectTypeDB::type_inherits_from(cname);
					} else {
						break;
					}
				}


				emit_signal("go_to_help","class_constant:"+result.class_name+":"+result.class_member);

			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_PROPERTY: {
				emit_signal("go_to_help","class_property:"+result.class_name+":"+result.class_member);

			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_METHOD: {

				StringName cname = result.class_name;

				while(true) {
					if (ObjectTypeDB::has_method(cname,result.class_member)) {
						result.class_name=cname;
						cname=ObjectTypeDB::type_inherits_from(cname);
					} else {
						break;
					}
				}

				emit_signal("go_to_help","class_method:"+result.class_name+":"+result.class_member);

			} break;
		}

	}
}

void ScriptTextEditor::_edit_option(int p_op) {

	switch(p_op) {
		case EDIT_UNDO: {
			code_editor->get_text_edit()->undo();
			code_editor->get_text_edit()->call_deferred("grab_focus");
		} break;
		case EDIT_REDO: {
			code_editor->get_text_edit()->redo();
			code_editor->get_text_edit()->call_deferred("grab_focus");
		} break;
		case EDIT_CUT: {

			code_editor->get_text_edit()->cut();
			code_editor->get_text_edit()->call_deferred("grab_focus");
		} break;
		case EDIT_COPY: {
			code_editor->get_text_edit()->copy();
			code_editor->get_text_edit()->call_deferred("grab_focus");

		} break;
		case EDIT_PASTE: {
			code_editor->get_text_edit()->paste();
			code_editor->get_text_edit()->call_deferred("grab_focus");

		} break;
		case EDIT_SELECT_ALL: {

			code_editor->get_text_edit()->select_all();
			code_editor->get_text_edit()->call_deferred("grab_focus");

		} break;
		case EDIT_MOVE_LINE_UP: {

			TextEdit *tx = code_editor->get_text_edit();
			Ref<Script> scr = script;
			if (scr.is_null())
				return;

			tx->begin_complex_operation();
			if (tx->is_selection_active())
			{
				int from_line = tx->get_selection_from_line();
				int from_col  = tx->get_selection_from_column();
				int to_line   = tx->get_selection_to_line();
				int to_column = tx->get_selection_to_column();

				for (int i = from_line; i <= to_line; i++)
				{
					int line_id = i;
					int next_id = i - 1;

					if (line_id == 0 || next_id < 0)
						return;

					swap_lines(tx, line_id, next_id);
				}
				int from_line_up = from_line > 0 ? from_line-1 : from_line;
				int to_line_up   = to_line   > 0 ? to_line-1   : to_line;
				tx->select(from_line_up, from_col, to_line_up, to_column);
			}
			else
			{
				int line_id = tx->cursor_get_line();
				int next_id = line_id - 1;

				if (line_id == 0 || next_id < 0)
					return;

				swap_lines(tx, line_id, next_id);
			}
			tx->end_complex_operation();
			tx->update();

		} break;
		case EDIT_MOVE_LINE_DOWN: {

			TextEdit *tx = code_editor->get_text_edit();
			Ref<Script> scr = get_edited_script();
			if (scr.is_null())
				return;

			tx->begin_complex_operation();
			if (tx->is_selection_active())
			{
				int from_line = tx->get_selection_from_line();
				int from_col  = tx->get_selection_from_column();
				int to_line   = tx->get_selection_to_line();
				int to_column = tx->get_selection_to_column();

				for (int i = to_line; i >= from_line; i--)
				{
					int line_id = i;
					int next_id = i + 1;

					if (line_id == tx->get_line_count()-1 || next_id > tx->get_line_count())
						return;

					swap_lines(tx, line_id, next_id);
				}
				int from_line_down = from_line < tx->get_line_count() ? from_line+1 : from_line;
				int to_line_down   = to_line   < tx->get_line_count() ? to_line+1   : to_line;
				tx->select(from_line_down, from_col, to_line_down, to_column);
			}
			else
			{
				int line_id = tx->cursor_get_line();
				int next_id = line_id + 1;

				if (line_id == tx->get_line_count()-1 || next_id > tx->get_line_count())
					return;

				swap_lines(tx, line_id, next_id);
			}
			tx->end_complex_operation();
			tx->update();

		} break;
		case EDIT_INDENT_LEFT: {

			TextEdit *tx = code_editor->get_text_edit();
			Ref<Script> scr = get_edited_script();
			if (scr.is_null())
				return;

			tx->begin_complex_operation();
			if (tx->is_selection_active())
			{
				tx->indent_selection_left();
			}
			else
			{
				int begin = tx->cursor_get_line();
				String line_text = tx->get_line(begin);
				// begins with tab
				if (line_text.begins_with("\t"))
				{
					line_text = line_text.substr(1, line_text.length());
					tx->set_line(begin, line_text);
				}
				// begins with 4 spaces
				else if (line_text.begins_with("    "))
				{
					line_text = line_text.substr(4, line_text.length());
					tx->set_line(begin, line_text);
				}
			}
			tx->end_complex_operation();
			tx->update();
			//tx->deselect();

		} break;
		case EDIT_INDENT_RIGHT: {

			TextEdit *tx = code_editor->get_text_edit();
			Ref<Script> scr = get_edited_script();
			if (scr.is_null())
				return;

			tx->begin_complex_operation();
			if (tx->is_selection_active())
			{
				tx->indent_selection_right();
			}
			else
			{
				int begin = tx->cursor_get_line();
				String line_text = tx->get_line(begin);
				line_text = '\t' + line_text;
				tx->set_line(begin, line_text);
			}
			tx->end_complex_operation();
			tx->update();
			//tx->deselect();

		} break;
		case EDIT_CLONE_DOWN: {

			TextEdit *tx = code_editor->get_text_edit();
			Ref<Script> scr = get_edited_script();
			if (scr.is_null())
				return;

			int from_line = tx->cursor_get_line();
			int to_line = tx->cursor_get_line();
			int column = tx->cursor_get_column();

			if (tx->is_selection_active()) {
				from_line = tx->get_selection_from_line();
				to_line = tx->get_selection_to_line();
				column = tx->cursor_get_column();
			}
			int next_line = to_line + 1;

			tx->begin_complex_operation();
			for (int i = from_line; i <= to_line; i++) {

				if (i >= tx->get_line_count() - 1) {
						tx->set_line(i, tx->get_line(i) + "\n");
				}
				String line_clone = tx->get_line(i);
				tx->insert_at(line_clone, next_line);
				next_line++;
			}

			tx->cursor_set_column(column);
			if (tx->is_selection_active()) {
				tx->select(to_line + 1, tx->get_selection_from_column(), next_line - 1, tx->get_selection_to_column());
			}

			tx->end_complex_operation();
			tx->update();

		} break;
		case EDIT_TOGGLE_COMMENT: {

			TextEdit *tx = code_editor->get_text_edit();
			Ref<Script> scr = get_edited_script();
			if (scr.is_null())
				return;


			tx->begin_complex_operation();
			if (tx->is_selection_active())
			{
				int begin = tx->get_selection_from_line();
				int end = tx->get_selection_to_line();

				// End of selection ends on the first column of the last line, ignore it.
				if(tx->get_selection_to_column() == 0)
					end -= 1;

				for (int i = begin; i <= end; i++)
				{
					String line_text = tx->get_line(i);

					if (line_text.begins_with("#"))
						line_text = line_text.substr(1, line_text.length());
					else
						line_text = "#" + line_text;
					tx->set_line(i, line_text);
				}
			}
			else
			{
				int begin = tx->cursor_get_line();
				String line_text = tx->get_line(begin);

				if (line_text.begins_with("#"))
					line_text = line_text.substr(1, line_text.length());
				else
					line_text = "#" + line_text;
				tx->set_line(begin, line_text);
			}
			tx->end_complex_operation();
			tx->update();
			//tx->deselect();

		} break;
		case EDIT_COMPLETE: {

			code_editor->get_text_edit()->query_code_comple();

		} break;
		case EDIT_AUTO_INDENT: {

			TextEdit *te = code_editor->get_text_edit();
			String text = te->get_text();
			Ref<Script> scr = get_edited_script();
			if (scr.is_null())
				return;
			int begin,end;
			if (te->is_selection_active()) {
				begin=te->get_selection_from_line();
				end=te->get_selection_to_line();
			} else {
				begin=0;
				end=te->get_line_count()-1;
			}
			scr->get_language()->auto_indent_code(text,begin,end);
			te->set_text(text);


		} break;
		case EDIT_TRIM_TRAILING_WHITESAPCE: {
			trim_trailing_whitespace();
		} break;
		case EDIT_PICK_COLOR: {
			color_panel->popup();
		} break;

		case SEARCH_CLASSES: {
			help_index->popup();
		} break;
		case SEARCH_FIND: {

			code_editor->get_find_replace_bar()->popup_search();
		} break;
		case SEARCH_FIND_NEXT: {

			code_editor->get_find_replace_bar()->search_next();
		} break;
		case SEARCH_FIND_PREV: {

			code_editor->get_find_replace_bar()->search_prev();
		} break;
		case SEARCH_REPLACE: {

			code_editor->get_find_replace_bar()->popup_replace();
		} break;
		case SEARCH_LOCATE_FUNCTION: {

			quick_open->popup(get_functions());
		} break;
		case SEARCH_GOTO_LINE: {

			goto_line_dialog->popup_find_line(code_editor->get_text_edit());
		} break;
		case DEBUG_TOGGLE_BREAKPOINT: {
			int line=code_editor->get_text_edit()->cursor_get_line();
			bool dobreak = !code_editor->get_text_edit()->is_line_set_as_breakpoint(line);
			code_editor->get_text_edit()->set_line_as_breakpoint(line,dobreak);
			ScriptEditor::get_singleton()->get_debugger()->set_breakpoint(get_edited_script()->get_path(),line+1,dobreak);
		} break;
		case DEBUG_REMOVE_ALL_BREAKPOINTS: {
			List<int> bpoints;
			code_editor->get_text_edit()->get_breakpoints(&bpoints);

			for(List<int>::Element *E=bpoints.front();E;E=E->next()) {
				int line = E->get();
				bool dobreak = !code_editor->get_text_edit()->is_line_set_as_breakpoint(line);
				code_editor->get_text_edit()->set_line_as_breakpoint(line,dobreak);
				ScriptEditor::get_singleton()->get_debugger()->set_breakpoint(get_edited_script()->get_path(),line+1,dobreak);
			}
		}
		case DEBUG_GOTO_NEXT_BREAKPOINT: {
			List<int> bpoints;
			code_editor->get_text_edit()->get_breakpoints(&bpoints);
			if (bpoints.size() <= 0) {
				return;
			}

			int line=code_editor->get_text_edit()->cursor_get_line();
			// wrap around
			if (line >= bpoints[bpoints.size() - 1]) {
				code_editor->get_text_edit()->cursor_set_line(bpoints[0]);
			} else {
				for(List<int>::Element *E=bpoints.front();E;E=E->next()) {
					int bline = E->get();
					if (bline > line) {
						code_editor->get_text_edit()->cursor_set_line(bline);
						return;
					}
				}
			}

		} break;
		case DEBUG_GOTO_PREV_BREAKPOINT: {
			List<int> bpoints;
			code_editor->get_text_edit()->get_breakpoints(&bpoints);
			if (bpoints.size() <= 0) {
				return;
			}

			int line=code_editor->get_text_edit()->cursor_get_line();
			// wrap around
			if (line <= bpoints[0]) {
				code_editor->get_text_edit()->cursor_set_line(bpoints[bpoints.size() - 1]);
			} else {
				for(List<int>::Element *E=bpoints.back();E;E=E->prev()) {
					int bline = E->get();
					if (bline < line) {
						code_editor->get_text_edit()->cursor_set_line(bline);
						return;
					}
				}
			}

		} break;

		case HELP_CONTEXTUAL: {
			String text = code_editor->get_text_edit()->get_selection_text();
			if (text == "")
				text = code_editor->get_text_edit()->get_word_under_cursor();
			if (text != "") {
				emit_signal("request_help_search",text);
			}
		} break;
	}
}

void ScriptTextEditor::_bind_methods() {

	ObjectTypeDB::bind_method("_validate_script",&ScriptTextEditor::_validate_script);
	ObjectTypeDB::bind_method("_load_theme_settings",&ScriptTextEditor::_load_theme_settings);
	ObjectTypeDB::bind_method("_breakpoint_toggled",&ScriptTextEditor::_breakpoint_toggled);
	ObjectTypeDB::bind_method("_edit_option",&ScriptTextEditor::_edit_option);
	ObjectTypeDB::bind_method("_goto_line",&ScriptTextEditor::_goto_line);
	ObjectTypeDB::bind_method("_lookup_symbol",&ScriptTextEditor::_lookup_symbol);
	ObjectTypeDB::bind_method("_text_edit_input_event", &ScriptTextEditor::_text_edit_input_event);
	ObjectTypeDB::bind_method("_color_changed", &ScriptTextEditor::_color_changed);


	ObjectTypeDB::bind_method("get_drag_data_fw",&ScriptTextEditor::get_drag_data_fw);
	ObjectTypeDB::bind_method("can_drop_data_fw",&ScriptTextEditor::can_drop_data_fw);
	ObjectTypeDB::bind_method("drop_data_fw",&ScriptTextEditor::drop_data_fw);

}

Control *ScriptTextEditor::get_edit_menu() {

	return edit_hb;
}

void ScriptTextEditor::reload(bool p_soft) {

	TextEdit *te = code_editor->get_text_edit();
	Ref<Script> scr = get_edited_script();
	if (scr.is_null())
		return;
	scr->set_source_code(te->get_text());
	bool soft =  p_soft || scr->get_instance_base_type()=="EditorPlugin"; //always soft-reload editor plugins

	scr->get_language()->reload_tool_script(scr,soft);
}

void ScriptTextEditor::get_breakpoints(List<int> *p_breakpoints) {

	code_editor->get_text_edit()->get_breakpoints(p_breakpoints);

}

void ScriptTextEditor::set_tooltip_request_func(String p_method,Object* p_obj) {

	code_editor->get_text_edit()->set_tooltip_request_func(p_obj,p_method,this);
}

void ScriptTextEditor::set_debugger_active(bool p_active) {


}


Variant ScriptTextEditor::get_drag_data_fw(const Point2& p_point,Control* p_from) {

	return Variant();
}

bool ScriptTextEditor::can_drop_data_fw(const Point2& p_point,const Variant& p_data,Control* p_from) const{

	Dictionary d = p_data;
	if (d.has("type") &&
			(

				String(d["type"])=="resource" ||
				String(d["type"])=="files" ||
				String(d["type"])=="nodes"
			) ) {


			return true;
	}


	return false;

}

#ifdef TOOLS_ENABLED

static Node* _find_script_node(Node* p_edited_scene,Node* p_current_node,const Ref<Script> &script) {

	if (p_edited_scene!=p_current_node && p_current_node->get_owner()!=p_edited_scene)
		return NULL;

	Ref<Script> scr = p_current_node->get_script();

	if (scr.is_valid() && scr==script)
		return p_current_node;

	for(int i=0;i<p_current_node->get_child_count();i++) {
		Node *n = _find_script_node(p_edited_scene,p_current_node->get_child(i),script);
		if (n)
			return n;
	}

	return NULL;
}

#else

static Node* _find_script_node(Node* p_edited_scene,Node* p_current_node,const Ref<Script> &script) {

	return NULL;
}
#endif




void ScriptTextEditor::drop_data_fw(const Point2& p_point,const Variant& p_data,Control* p_from){

	Dictionary d = p_data;

	if (d.has("type") && String(d["type"])=="resource") {

		Ref<Resource> res = d["resource"];
		if (!res.is_valid()) {
			return;
		}

		if (res->get_path().is_resource_file()) {
			EditorNode::get_singleton()->show_warning("Only resources from filesystem can be dropped.");
			return;
		}

		code_editor->get_text_edit()->insert_text_at_cursor(res->get_path());

	}

	if (d.has("type") && String(d["type"])=="files") {


		Array files = d["files"];

		String text_to_drop;
		for(int i=0;i<files.size();i++) {

			if (i>0)
				text_to_drop+=",";
			text_to_drop+="\""+String(files[i]).c_escape()+"\"";

		}

		code_editor->get_text_edit()->insert_text_at_cursor(text_to_drop);

	}

	if (d.has("type") && String(d["type"])=="nodes") {

		Node* sn = _find_script_node(get_tree()->get_edited_scene_root(),get_tree()->get_edited_scene_root(),script);


		if (!sn) {
			EditorNode::get_singleton()->show_warning("Can't drop nodes because script '"+get_name()+"' is not used in this scene.");
			return;
		}


		Array nodes = d["nodes"];
		String text_to_drop;
		for(int i=0;i<nodes.size();i++) {

			if (i>0)
				text_to_drop+=",";

			NodePath np = nodes[i];
			Node *node = get_node(np);
			if (!node) {
				continue;
			}



			String path = sn->get_path_to(node);
			text_to_drop+="\""+path.c_escape()+"\"";


		}

		code_editor->get_text_edit()->insert_text_at_cursor(text_to_drop);


	}



}

void ScriptTextEditor::_text_edit_input_event(const InputEvent& ev) {
	if (ev.type == InputEvent::MOUSE_BUTTON) {
		InputEventMouseButton mb = ev.mouse_button;
		if (mb.button_index == BUTTON_RIGHT && !mb.pressed) {

			int col, row;
			TextEdit* tx = code_editor->get_text_edit();
			tx->_get_mouse_pos(Point2i(mb.global_x, mb.global_y)-tx->get_global_pos(), row, col);
			Vector2 mpos = Vector2(mb.global_x, mb.global_y)-tx->get_global_pos();
			bool have_selection = (tx->get_selection_text().length() > 0);
			bool have_color = (tx->get_word_at_pos(mpos) == "Color");
			if (have_color) {

				String line = tx->get_line(row);
				color_line = row;
				int begin = 0;
				int end   = 0;
				bool valid = false;
				for (int i = col; i < line.length(); i++) {
					if (line[i] == '(') {
						begin = i;
						continue;
					}
					else if (line[i] == ')') {
						end = i+1;
						valid = true;
						break;
					}
				}
				if (valid) {
					color_args = line.substr(begin, end-begin);
					String stripped = color_args.replace(" ", "").replace("(", "").replace(")", "");
					Vector<float> color = stripped.split_floats(",");
					if (color.size() > 2) {
						float alpha = color.size() > 3 ? color[3] : 1.0f;
						color_picker->set_color(Color(color[0], color[1], color[2], alpha));
					}
					color_panel->set_pos(get_global_transform().xform(get_local_mouse_pos()));
					Size2 ms = Size2(300, color_picker->get_combined_minimum_size().height+10);
					color_panel->set_size(ms);
				} else {
					have_color = false;
				}
			}
			_make_context_menu(have_selection, have_color);
		}
	}
}

void ScriptTextEditor::_color_changed(const Color& p_color) {
	String new_args;
	if (p_color.a == 1.0f) {
		new_args = String("("+rtos(p_color.r)+", "+rtos(p_color.g)+", "+rtos(p_color.b)+")");
	} else {
		new_args = String("("+rtos(p_color.r)+", "+rtos(p_color.g)+", "+rtos(p_color.b)+", "+rtos(p_color.a)+")");
	}

	String line = code_editor->get_text_edit()->get_line(color_line);
	String new_line = line.replace(color_args, new_args);
	color_args = new_args;
	code_editor->get_text_edit()->set_line(color_line, new_line);
}

void ScriptTextEditor::_make_context_menu(bool p_selection, bool p_color) {

	context_menu->clear();
	if (p_selection) {
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/cut"));
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/copy"));
	}

	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/paste"));
	context_menu->add_separator();
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/select_all"));
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/undo"));

	if (p_selection) {
		context_menu->add_separator();
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_left"));
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_right"));
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_comment"));
	}
	if (p_color) {
		context_menu->add_separator();
		context_menu->add_item(TTR("Pick Color"), EDIT_PICK_COLOR);
	}
	context_menu->set_pos(get_global_transform().xform(get_local_mouse_pos()));
	context_menu->set_size(Vector2(1, 1));
	context_menu->popup();
}

ScriptTextEditor::ScriptTextEditor() {

	code_editor = memnew( CodeTextEditor );
	add_child(code_editor);
	code_editor->set_area_as_parent_rect();
	code_editor->connect("validate_script",this,"_validate_script");
	code_editor->connect("load_theme_settings",this,"_load_theme_settings");
	code_editor->set_code_complete_func(_code_complete_scripts,this);
	code_editor->get_text_edit()->connect("breakpoint_toggled", this, "_breakpoint_toggled");
	code_editor->get_text_edit()->connect("symbol_lookup", this, "_lookup_symbol");

	update_settings();

	code_editor->get_text_edit()->set_callhint_settings(
		EditorSettings::get_singleton()->get("text_editor/put_callhint_tooltip_below_current_line"),
		EditorSettings::get_singleton()->get("text_editor/callhint_tooltip_offset"));

	code_editor->get_text_edit()->set_select_identifiers_on_hover(true);
	code_editor->get_text_edit()->set_context_menu_enabled(false);
	code_editor->get_text_edit()->connect("input_event", this, "_text_edit_input_event");

	context_menu = memnew(PopupMenu);
	add_child(context_menu);
	context_menu->connect("item_pressed", this, "_edit_option");

	color_panel = memnew(PopupPanel);
	add_child(color_panel);
	color_picker = memnew(ColorPicker);
	color_panel->add_child(color_picker);
	color_panel->set_child_rect(color_picker);
	color_picker->connect("color_changed", this, "_color_changed");

	edit_hb = memnew (HBoxContainer);

	edit_menu = memnew( MenuButton );
	edit_menu->set_text(TTR("Edit"));
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/undo"), EDIT_UNDO);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/redo"), EDIT_REDO);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/cut"), EDIT_CUT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/copy"), EDIT_COPY);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/paste"), EDIT_PASTE);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/select_all"), EDIT_SELECT_ALL);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/move_up"), EDIT_MOVE_LINE_UP);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/move_down"), EDIT_MOVE_LINE_DOWN);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_left"), EDIT_INDENT_LEFT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_right"), EDIT_INDENT_RIGHT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_comment"), EDIT_TOGGLE_COMMENT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/clone_down"), EDIT_CLONE_DOWN);
	edit_menu->get_popup()->add_separator();
#ifdef OSX_ENABLED
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/complete_symbol"), EDIT_COMPLETE);
#else
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/complete_symbol"), EDIT_COMPLETE);
#endif
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/trim_trailing_whitespace"), EDIT_TRIM_TRAILING_WHITESAPCE);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/auto_indent"), EDIT_AUTO_INDENT);
	edit_menu->get_popup()->connect("item_pressed", this,"_edit_option");
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_breakpoint"), DEBUG_TOGGLE_BREAKPOINT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/remove_all_breakpoints"), DEBUG_REMOVE_ALL_BREAKPOINTS);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_next_breakpoint"), DEBUG_GOTO_NEXT_BREAKPOINT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_previous_breakpoint"), DEBUG_GOTO_PREV_BREAKPOINT);

	search_menu = memnew( MenuButton );
	edit_hb->add_child(search_menu);
	search_menu->set_text(TTR("Search"));
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/classes"), SEARCH_CLASSES);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find"), SEARCH_FIND);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find_next"), SEARCH_FIND_NEXT);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find_previous"), SEARCH_FIND_PREV);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/replace"), SEARCH_REPLACE);
	search_menu->get_popup()->add_separator();
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_function"), SEARCH_LOCATE_FUNCTION);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_line"), SEARCH_GOTO_LINE);
	search_menu->get_popup()->add_separator();
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/contextual_help"), HELP_CONTEXTUAL);

	search_menu->get_popup()->connect("item_pressed", this,"_edit_option");

	edit_hb->add_child(edit_menu);

	quick_open = memnew( ScriptEditorQuickOpen );
	add_child(quick_open);
	quick_open->connect("goto_line",this,"_goto_line");

	goto_line_dialog = memnew(GotoLineDialog);
	add_child(goto_line_dialog);


	code_editor->get_text_edit()->set_drag_forwarding(this);

	help_index = memnew( EditorHelpIndex );
	add_child(help_index);
	help_index->connect("open_class",this,"_help_class_open");
}

static ScriptEditorBase * create_editor(const Ref<Script>& p_script) {

	if (p_script->has_source_code()) {
		return memnew( ScriptTextEditor );
	}

	return NULL;
}

void ScriptTextEditor::register_editor() {

	ED_SHORTCUT("script_text_editor/undo", TTR("Undo"), KEY_MASK_CMD|KEY_Z);
	ED_SHORTCUT("script_text_editor/redo", TTR("Redo"), KEY_MASK_CMD|KEY_Y);
	ED_SHORTCUT("script_text_editor/cut", TTR("Cut"), KEY_MASK_CMD|KEY_X);
	ED_SHORTCUT("script_text_editor/copy", TTR("Copy"), KEY_MASK_CMD|KEY_C);
	ED_SHORTCUT("script_text_editor/paste", TTR("Paste"), KEY_MASK_CMD|KEY_V);
	ED_SHORTCUT("script_text_editor/select_all", TTR("Select All"), KEY_MASK_CMD|KEY_A);
	ED_SHORTCUT("script_text_editor/move_up", TTR("Move Up"), KEY_MASK_ALT|KEY_UP);
	ED_SHORTCUT("script_text_editor/move_down", TTR("Move Down"), KEY_MASK_ALT|KEY_DOWN);
	ED_SHORTCUT("script_text_editor/indent_left", TTR("Indent Left"), KEY_MASK_ALT|KEY_LEFT);
	ED_SHORTCUT("script_text_editor/indent_right", TTR("Indent Right"), KEY_MASK_ALT|KEY_RIGHT);
	ED_SHORTCUT("script_text_editor/toggle_comment", TTR("Toggle Comment"), KEY_MASK_CMD|KEY_K);
	ED_SHORTCUT("script_text_editor/clone_down", TTR("Clone Down"), KEY_MASK_CMD|KEY_B);
#ifdef OSX_ENABLED
	ED_SHORTCUT("script_text_editor/complete_symbol", TTR("Complete Symbol"), KEY_MASK_CTRL|KEY_SPACE);
#else
	ED_SHORTCUT("script_text_editor/complete_symbol", TTR("Complete Symbol"), KEY_MASK_CMD|KEY_SPACE);
#endif
	ED_SHORTCUT("script_text_editor/trim_trailing_whitespace", TTR("Trim Trailing Whitespace"), KEY_MASK_CTRL|KEY_MASK_ALT|KEY_T);
	ED_SHORTCUT("script_text_editor/auto_indent", TTR("Auto Indent"), KEY_MASK_CMD|KEY_I);

	ED_SHORTCUT("script_text_editor/toggle_breakpoint", TTR("Toggle Breakpoint"), KEY_F9);
	ED_SHORTCUT("script_text_editor/remove_all_breakpoints", TTR("Remove All Breakpoints"), KEY_MASK_CTRL|KEY_MASK_SHIFT|KEY_F9);
	ED_SHORTCUT("script_text_editor/goto_next_breakpoint", TTR("Goto Next Breakpoint"), KEY_MASK_CTRL|KEY_PERIOD);
	ED_SHORTCUT("script_text_editor/goto_previous_breakpoint", TTR("Goto Previous Breakpoint"), KEY_MASK_CTRL|KEY_COMMA);

	ED_SHORTCUT("script_text_editor/classes", TTR("Classes"), KEY_MASK_CMD|KEY_P);
	ED_SHORTCUT("script_text_editor/find", TTR("Find.."), KEY_MASK_CMD|KEY_F);
	ED_SHORTCUT("script_text_editor/find_next", TTR("Find Next"), KEY_F3);
	ED_SHORTCUT("script_text_editor/find_previous", TTR("Find Previous"), KEY_MASK_SHIFT|KEY_F3);
	ED_SHORTCUT("script_text_editor/replace", TTR("Replace.."), KEY_MASK_CMD|KEY_R);

	ED_SHORTCUT("script_text_editor/goto_function", TTR("Goto Function.."), KEY_MASK_SHIFT|KEY_MASK_CMD|KEY_F);
	ED_SHORTCUT("script_text_editor/goto_line", TTR("Goto Line.."), KEY_MASK_CMD|KEY_L);

	ED_SHORTCUT("script_text_editor/contextual_help", TTR("Contextual Help"), KEY_MASK_SHIFT|KEY_F1);

	ScriptEditor::register_create_script_editor_function(create_editor);
}
