<?php

/**
 * TODO: label and move this before committing.
 *
 * TODO: This is essentially lifted straight from views as it stands right now.  This isn't *really* what we want to do.
 *
 * Decide what to do about exporting options that are on activities themselves.  Currently these are member variables on activities.
 */
class ConductorObject {

  /**
   * Information about options for all kinds of purposes will be held here.
   * @code
   * 'option_name' => array(
   *  - 'default' => default value,
   *  - 'translatable' => TRUE/FALSE (wrap in t() on export if true),
   *  - 'contains' => array of items this contains, with its own defaults, etc.
   *      If contains is set, the default will be ignored and assumed to
   *      be array()
   *
   *  ),
   *  @endcode
   * Each option may have any of the following functions:
   *  - export_option_OPTIONNAME -- Special export handling if necessary.
   *  - translate_option_OPTIONNAME -- Special handling for translating data
   *    within the option, if necessary.
   */
  function option_definition() { return array(); }

  function export_options($indent, $prefix) {
    $output = '';
    foreach ($this->option_definition() as $option => $definition) {
      if (isset($this->$option) && $this->$option != $definition['default']) {
        $output .= $prefix . '->' . $option . ' = ' . ctools_var_export($this->$option) . ";\n";
        // TODO: something like below?
        //$output .= $this->export_option($indent, $prefix, $this->$option, $option, $definition, array());
      }
    }

    return $output;
  }

  function export_option($indent, $prefix, $storage, $option, $definition, $parents) {
    // Do not export options for which we have no settings.
    if (!isset($storage[$option])) {
      return;
    }

    if (isset($definition['export'])) {
      if ($definition['export'] === FALSE) {
        return;
      }

      // Special handling for some items
      if (method_exists($this, $definition['export'])) {
        return $this->{$definition['export']}($indent, $prefix, $storage, $option, $definition, $parents);
      }
    }

    // Add the current option to the parents tree.
    $parents[] = $option;
    $output = '';

    // If it has child items, export those separately.
    if (isset($definition['contains'])) {
      foreach ($definition['contains'] as $sub_option => $sub_definition) {
        $output .= $this->export_option($indent, $prefix, $storage[$option], $sub_option, $sub_definition, $parents);
      }
    }
    // Otherwise export just this item.
    else {
      $default = isset($definition['default']) ? $definition['default'] : NULL;
      $value = $storage[$option];
      if (isset($definition['bool'])) {
        $value = (bool) $value;
      }

      if ($value !== $default) {
        $output .= $indent . $prefix . "['" . implode("']['", $parents) . "'] = ";
        if (isset($definition['bool'])) {
          $output .= empty($storage[$option]) ? 'FALSE' : 'TRUE';
        }
        else {
          $output .= views_var_export($storage[$option], $indent);
        }

        $output .= ";\n";
      }
    }
    return $output;
  }
}
