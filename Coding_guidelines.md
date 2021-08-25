# Coding guidelines
This document is intended to encourage good coding practices 
without inhibiting progress. Rules herein should be open ended 
to allow the author to "do better". 

The main rules in order of importance:
1. Readability and maintenance.
2. Robustness.
3. Performance.

Old code is often breaking rules and should be rewritten when time permits.
## Hard rules
Indent with tabs. 

## Readability 
* Ensure horizontal and vertical whitespace.
* Reduce complexity.
* camelCase -> snake_case.
* Remove noise
  * ::get_name() -> ::name()
  * liberal use of "using namespace" 
  * Does it add value? If not remove.
  * Comments clutter code. Rewrite code so that it is clear
* Code should visually convey information.
  * Form parameters as lists.
  * Symmetry.
    * Starting brace on same column as ending brace (or on same line).
    * Variables in columns.
  

## robustness
* Immutable.
* Avoid pointers.
* Pass by value.
* No naked new.
* No null.
* No **unscoped** "using namespace" in header files

## Conventions
* Indent with tabs.
* snake_case.
* lower case filenames.
* No prefix or postfix on variables and fields if possible. 
  * If fields and method names collide we concatenate "_m" on the field.
  * If parameter and field name collides concatenate "op_" on parameter (operator).
  


## Code examples

### Example 1
<pre>
void preference_dialog::on_data_changed()
{
  get_values();
  if (gz < gt
    &&
    (
      gz              != old_gz              ||
      gt              != old_gt              ||
      compact_layout  != old_compact_layout  ||
      start_minimized != old_start_minimized ||
      password        != old_password        ||
      user            != old_user            ||
      url             != old_url             ||
      ignore_cert_err != old_ignore_cert_err ||
      sync_interval   != old_sync_interval   ||
      quiet_mode      != old_quiet_mode
    ))
  {
    ok_button.set_sensitive(true);
  }
  else
  {
    ok_button.set_sensitive(false);
  }
}
</pre>

### Example 1
<pre>
bool arbitrary_function(
  float    param1,
  float    param2,
  long int param3)
{
  constexpr auto pi = 3.141592

  auto result = param1 * param2 * pi;
  return (result < param3);
}
</pre>