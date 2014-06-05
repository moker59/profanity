/*
 * ruby_api.c
 *
 * Copyright (C) 2012 - 2014 James Booth <boothj5@gmail.com>
 *
 * This file is part of Profanity.
 *
 * Profanity is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Profanity is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Profanity.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <ruby.h>

#include <glib.h>

#include "plugins/api.h"
#include "plugins/ruby_api.h"
#include "plugins/callbacks.h"

static VALUE
ruby_api_cons_alert(VALUE self)
{
    api_cons_alert();
    return Qnil;
}

static VALUE
ruby_api_cons_show(VALUE self, VALUE v_message)
{
    char *message = STR2CSTR(v_message);

    if (message != NULL) {
        api_cons_show(message);
    }
    return Qnil;
}

static VALUE
ruby_api_register_command(VALUE self, VALUE v_command_name, VALUE v_min_args,
    VALUE v_max_args, VALUE v_usage, VALUE v_short_help, VALUE v_long_help,
    VALUE v_callback)
{
    const char *command_name = STR2CSTR(v_command_name);
    int min_args = NUM2INT(v_min_args);
    int max_args = NUM2INT(v_max_args);
    const char *usage = STR2CSTR(v_usage);
    const char *short_help = STR2CSTR(v_short_help);
    const char *long_help = STR2CSTR(v_long_help);

    api_register_command(command_name, min_args, max_args, usage,
        short_help, long_help, (void *)v_callback, ruby_command_callback);

    return Qnil;
}

static VALUE
ruby_api_register_timed(VALUE self, VALUE v_callback, VALUE v_interval_seconds)
{
    int interval_seconds = NUM2INT(v_interval_seconds);

    api_register_timed((void*)v_callback, interval_seconds, ruby_timed_callback);

    return Qnil;
}

static VALUE
ruby_api_notify(VALUE self, VALUE v_message, VALUE v_timeout_ms, VALUE v_category)
{
    const char *message = STR2CSTR(v_message);
    const char *category = STR2CSTR(v_category);
    int timeout_ms = NUM2INT(v_timeout_ms);

    api_notify(message, category, timeout_ms);

    return Qnil;
}

static VALUE
ruby_api_send_line(VALUE self, VALUE v_line)
{
    char *line = STR2CSTR(v_line);

    api_send_line(line);

    return Qnil;
}

static VALUE
ruby_api_get_current_recipient(VALUE self)
{
    char *recipient = api_get_current_recipient();
    if (recipient != NULL) {
        return rb_str_new2(recipient);
    } else {
        return Qnil;
    }
}

static VALUE
ruby_api_log_debug(VALUE self, VALUE v_message)
{
    char *message = STR2CSTR(v_message);

    if (message != NULL) {
        api_log_debug(message);
    }
    return Qnil;
}

static VALUE
ruby_api_log_info(VALUE self, VALUE v_message)
{
    char *message = STR2CSTR(v_message);

    if (message != NULL) {
        api_log_info(message);
    }
    return Qnil;
}

static VALUE
ruby_api_log_warning(VALUE self, VALUE v_message)
{
    char *message = STR2CSTR(v_message);

    if (message != NULL) {
        api_log_warning(message);
    }
    return Qnil;
}

static VALUE
ruby_api_log_error(VALUE self, VALUE v_message)
{
    char *message = STR2CSTR(v_message);

    if (message != NULL) {
        api_log_error(message);
    }
    return Qnil;
}

static VALUE
ruby_api_win_exists(VALUE self, VALUE v_tag)
{
    char *tag = STR2CSTR(v_tag);

    if (api_win_exists(tag)) {
        return Qtrue;
    } else {
        return Qfalse;
    }
}

static VALUE
ruby_api_win_create(VALUE self, VALUE v_tag, VALUE v_callback)
{
    char *tag = STR2CSTR(v_tag);

    api_win_create(tag, (void*)v_callback, ruby_window_callback);

    return Qnil;
}

static VALUE
ruby_api_win_focus(VALUE self, VALUE v_tag)
{
    char *tag = STR2CSTR(v_tag);

    api_win_focus(tag);
    return Qnil;
}

static VALUE
ruby_api_win_process_line(VALUE self, VALUE v_tag, VALUE v_line)
{
    char *tag = STR2CSTR(v_tag);
    char *line = STR2CSTR(v_line);

    api_win_process_line(tag, line);
    return Qnil;
}

static VALUE
ruby_api_win_show(VALUE self, VALUE v_tag, VALUE v_line)
{
    char *tag = STR2CSTR(v_tag);
    char *line = STR2CSTR(v_line);

    api_win_show(tag, line);
    return Qnil;
}

static VALUE
ruby_api_win_show_green(VALUE self, VALUE v_tag, VALUE v_line)
{
    char *tag = STR2CSTR(v_tag);
    char *line = STR2CSTR(v_line);

    api_win_show_green(tag, line);
    return Qnil;
}

static VALUE
ruby_api_win_show_red(VALUE self, VALUE v_tag, VALUE v_line)
{
    char *tag = STR2CSTR(v_tag);
    char *line = STR2CSTR(v_line);

    api_win_show_red(tag, line);
    return Qnil;
}

static VALUE
ruby_api_win_show_cyan(VALUE self, VALUE v_tag, VALUE v_line)
{
    char *tag = STR2CSTR(v_tag);
    char *line = STR2CSTR(v_line);

    api_win_show_cyan(tag, line);
    return Qnil;
}

void
ruby_command_callback(PluginCommand *command, gchar **args)
{
    int num_args = g_strv_length(args);
    if (num_args == 0) {
        if (command->max_args == 1) {
            rb_funcall((VALUE)command->callback, rb_intern("call"), 1, Qnil);
        } else {
            rb_funcall((VALUE)command->callback, rb_intern("call"), 0);
        }
    } else if (num_args == 1) {
        rb_funcall((VALUE)command->callback, rb_intern("call"), 1,
            rb_str_new2(args[0]));
    } else if (num_args == 2) {
        rb_funcall((VALUE)command->callback, rb_intern("call"), 1,
            rb_str_new2(args[0]), rb_str_new2(args[1]));
    } else if (num_args == 3) {
        rb_funcall((VALUE)command->callback, rb_intern("call"), 1,
            rb_str_new2(args[0]), rb_str_new2(args[1]), rb_str_new2(args[2]));
    } else if (num_args == 4) {
        rb_funcall((VALUE)command->callback, rb_intern("call"), 1,
            rb_str_new2(args[0]), rb_str_new2(args[1]), rb_str_new2(args[2]),
            rb_str_new2(args[3]));
    } else if (num_args == 5) {
        rb_funcall((VALUE)command->callback, rb_intern("call"), 1,
            rb_str_new2(args[0]), rb_str_new2(args[1]), rb_str_new2(args[2]),
            rb_str_new2(args[3]), rb_str_new2(args[4]));
    }
}

void
ruby_timed_callback(PluginTimedFunction *timed_function)
{
    rb_funcall((VALUE)timed_function->callback, rb_intern("call"), 0);
}

void
ruby_window_callback(PluginWindowCallback *window_callback, char *tag, char *line)
{
    rb_funcall((VALUE)window_callback->callback, rb_intern("call"), 2,
            rb_str_new2(tag), rb_str_new2(line));
}

static VALUE prof_module;

void
ruby_api_init(void)
{
    prof_module = rb_define_module("Prof");

    rb_define_module_function(prof_module, "cons_alert", RUBY_METHOD_FUNC(ruby_api_cons_alert), 0);
    rb_define_module_function(prof_module, "cons_show", RUBY_METHOD_FUNC(ruby_api_cons_show), 1);
    rb_define_module_function(prof_module, "register_command", RUBY_METHOD_FUNC(ruby_api_register_command), 7);
    rb_define_module_function(prof_module, "register_timed", RUBY_METHOD_FUNC(ruby_api_register_timed), 2);
    rb_define_module_function(prof_module, "send_line", RUBY_METHOD_FUNC(ruby_api_send_line), 1);
    rb_define_module_function(prof_module, "notify", RUBY_METHOD_FUNC(ruby_api_notify), 3);
    rb_define_module_function(prof_module, "get_current_recipient", RUBY_METHOD_FUNC(ruby_api_get_current_recipient), 0);
    rb_define_module_function(prof_module, "log_debug", RUBY_METHOD_FUNC(ruby_api_log_debug), 1);
    rb_define_module_function(prof_module, "log_info", RUBY_METHOD_FUNC(ruby_api_log_info), 1);
    rb_define_module_function(prof_module, "log_warning", RUBY_METHOD_FUNC(ruby_api_log_warning), 1);
    rb_define_module_function(prof_module, "log_error", RUBY_METHOD_FUNC(ruby_api_log_error), 1);
    rb_define_module_function(prof_module, "win_exists", RUBY_METHOD_FUNC(ruby_api_win_exists), 1);
    rb_define_module_function(prof_module, "win_create", RUBY_METHOD_FUNC(ruby_api_win_create), 2);
    rb_define_module_function(prof_module, "win_focus", RUBY_METHOD_FUNC(ruby_api_win_focus), 1);
    rb_define_module_function(prof_module, "win_process_line", RUBY_METHOD_FUNC(ruby_api_win_process_line), 2);
    rb_define_module_function(prof_module, "win_show", RUBY_METHOD_FUNC(ruby_api_win_show), 2);
    rb_define_module_function(prof_module, "win_show_green", RUBY_METHOD_FUNC(ruby_api_win_show_green), 2);
    rb_define_module_function(prof_module, "win_show_red", RUBY_METHOD_FUNC(ruby_api_win_show_red), 2);
    rb_define_module_function(prof_module, "win_show_cyan", RUBY_METHOD_FUNC(ruby_api_win_show_cyan), 2);
}
