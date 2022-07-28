<?php
/**
 * @file
 * Snippet to be placed in appropriate settings file for toggling between read-only and read/write configuration mode
 */

// This should be placed after the final $config_directories declaration in your settings.php or local settings file
$config_directories['active'] = 'private/config/' . $site;
if (file_exists($config_directories['active'] . '/pantheon_config_deploy.settings.json')) {
  if ($pantheon_config_deploy_config = file_get_contents($config_directories['active'] . '/pantheon_config_deploy.settings.json')) {
    $pantheon_config_deploy_config = json_decode($pantheon_config_deploy_config);
    if (isset($pantheon_config_deploy_config->writeable_config) && !empty($pantheon_config_deploy_config->writeable_config)) {
      $wconfig = $pantheon_config_deploy_config->writeable_config;
    }
  }
}
if (!isset($wconfig) || empty($wconfig)) {
  $wconfig = 'files/config';
}
if (defined('BACKDROP_ROOT')) {
  if (file_exists(BACKDROP_ROOT . '/' . $wconfig . '/temp_active.true')) {
    $scandir = scandir(BACKDROP_ROOT . '/' . $wconfig);
    if (count($scandir) == 4) {
      foreach ($scandir as $dir_file) {
        if (strpos($dir_file, "config_") === 0) {
          $config_directories['staging'] = $config_directories['active'];
          $config_directories['active'] = $wconfig . '/' . $dir_file;
        }
      }
    }
  }
}