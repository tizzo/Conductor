<?php

$plugin = array(
  'schema' => 'conductor_workflow',
  'access' => 'administer conductor workflows',

  'menu' => array(
    'menu prefix' => 'admin/structure',
    'menu item' => 'conductor',
    'menu title' => 'Conductor Workflows', 
    'menu description' => 'Manage custom workflows',
  ),

  'title singular' => t('workflow'),
  'title singular proper' => t('Conductor Workflow'),
  'title plural' => t('workflows'),
  'title plural proper' => t('Conductor Workflows'),

  // This will provide you with a form for editing the properties on your
  // exportable, with validate and submit handler.
  //
  // The item being edited will be in $form_state['item'].
  //
  // The submit handler is only responsible for moving data from
  // $form_state['values'] to $form_state['item'].
  //
  // All callbacks will accept &$form and &$form_state as arguments.
  'form' => array(
    //'settings' => 'conductor_ui_ctools_export_ui_form',
    //'validate' => 'conductor_ui_ctools_export_ui_form_validate',
    //'submit' => 'conductor_ui_ctools_export_ui_form_submit',
  ),

  'strings' => array(
    'confirmation' => array(
      'revert' => array(
        'information' => t('This action will permanently remove any customizations made to this view.'),
        'success' => t('The view has been reverted.'),
      ),
      'delete' => array(
        'information' => t('This action will permanently remove the view from your database.'),
        'success' => t('The view has been deleted.'),
      ),
    ),
  ),

  'handler' => array(
     'class' => 'conductor_ui',
     'parent' => 'ctools_export_ui',
     'file' => 'conductor_ui.class.inc',
   ),

);
