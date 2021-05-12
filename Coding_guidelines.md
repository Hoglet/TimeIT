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
* Form parameters as lists.
* Symmetry. 
* Reduce complexity.
* camelCase -> snake_case.
* Remove noise
    * ::getName() -> ::name()
    * liberal use of "using namespace" 

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
* lover case filenames.
* No prefix or postfix on variables and fields if possible. 
  * If fields and method names collide we concatenate "_" on the field.
  * If parameter and field name collides concatenate "_" on parameter.
  


## Code example
<pre>
void Preference_dialog::on_data_changed()
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