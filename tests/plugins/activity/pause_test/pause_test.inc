<?php

$plugin = array(
  'title' => t('Pause Test'),
  'description' => t('This activity pauses a workflow and resumes only if it is resumed with a context of \'pause_test\' => \'resume\''),
  'handler' => array(
    'class' => 'ConductorActivityPauseTest',
  ),
);
