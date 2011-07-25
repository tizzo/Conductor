<?php

/**
 * @file
 *   Documents 
 */

/**
 * Implements hook_conductor_activity_process().
 *
 * Allows modules to modify the 
 *
 * @param $plugin
 *   The ctools plugin data.
 */
function hook_conductor_activity_process(&$plugin) {
  $plugin += array(
    'foo' => 'bar',
  );
}
