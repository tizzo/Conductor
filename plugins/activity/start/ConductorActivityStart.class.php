<?php

/**
 * This is the first activity in any workflow.
 */
class ConductorActivityStart extends ConductorActivity {

  public function option_definition() {
    $options = parent::option_definition();
    unset($options['inputs']);
    return $options;
  }

  public function addInput($activity) {
    return FALSE;
  }

  /**
   * The Start action has no dependencies.
   */
  public function checkRunnability() {
    return TRUE;
  }

  /**
   * The start method performs no actions.
   */
  public function process() {
    return TRUE;
  }

  /**
   * Provide UI links, disabling those from the parent class that do not apply.
   */
  public function getUILinks() {
    // Create an array to be rendered by theme_links__ctools_dropbutton().
    $links = parent::getUILinks();
    unset($links['input']);
    unset($links['remove']);
    return $links;
  }

}
