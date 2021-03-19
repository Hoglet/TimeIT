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
* No smart pointers if not really needed.
* Pass by value.
* No raw pointers 
* No naked new.
* No null.
* No **unscoped** "using namespace" in header files


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
      URL             != old_URL             ||
      ignore_cert_err != old_ignore_cert_err ||
      sync_interval   != old_sync_interval   ||
      quietMode       != old_quiet_mode
    ))
  {
    OK_button.set_sensitive(true);
  }
  else
  {
    OK_button.set_sensitive(false);
  }
}
</pre>