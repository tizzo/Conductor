<?php

/**
 *
 */
class ConductorEnd extends ConductorActivity {

  public function option_definition() {
    $options = parent::option_definition();
    unset($options['outputs']);
    return $options;
  }

  public function addOutput($activity) {
    return FALSE;
  }

  public function run() {
    $state = parent::run();
    if ($state->status() === ConductorActivityState::FINISHED) {
      //$this->state->workflowComplete();
    }
    return $state;
  }

  /**
   *
   */
  public function getUILinks() {
    // Create an array to be rendered by theme_links__ctools_dropbutton().
    $links = parent::getUILinks();
    unset($links['output']);
    unset($links['remove']);
    //unset($links['edit']);
    return $links;
  }

}
