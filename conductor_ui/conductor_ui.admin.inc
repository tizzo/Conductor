<?php

/**
 * @file
 *   Provides administrative pages and resources for conductor_ui.
 *
 * Note that most of conductor_ui lives in the exportable ui plugin found
 * inside `plugins/export_ui/conductor_ui.class.inc`.

/**
 * Provides the form to manage conductor global default behaviors.
 */
function conductor_ui_admin_settings_form() {
  $form = array();
  return system_settings_form($form);
}
