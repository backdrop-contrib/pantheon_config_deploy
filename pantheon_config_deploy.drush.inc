<?php
/**
 * @file
 * Defines drush utilities for managing configuration on Pantheon environments
 */

/**
 * Implements hook_drush_command().
 */
function pantheon_config_deploy_drush_command() {

  $items = array();
  $items['pc-remove-config'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_pantheon_config_deploy_drush_remove_config',  // Callback function
    'description' => 'Removes the writeable configuration directory in the Pantheon filesystem, and turns write mode OFF.',
    'aliases' => array('pcrc'), // alias of command
    'examples' => array(      // List these example when user types : drush help nc
      'Remove writeable config and set writeable mode to OFF' => 'drush pcrc',
    ),
  );
  $items['pc-clone-config'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_pantheon_config_deploy_drush_clone_config',  // Callback function
    'description' => 'Clones configuration into a writeable directory in the Pantheon filesystem.',
    'aliases' => array('pccc'), // alias of command
    'examples' => array(      // List these example when user types : drush help nc
      'Create a new writeable config instance' => 'drush pccc',
    ),
  );
  $items['pc-wconfig'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_pantheon_config_deploy_drush_wconfig',  // Callback function
    'description' => 'Sets writeable config mode to true or false.',
    'aliases' => array('pcwc'), // alias of command
    'examples' => array(      // List these example when user types : drush help nc
      'Set writeable config mode to On' => 'drush pcwc 1',
      'Set writeable config mode to Off' => 'drush pcwc 0',
    ),
  );
  return $items;
}

/**
 * Clones config to a writeable directory via drush
 */
function drush_pantheon_config_deploy_drush_wconfig($state = 0) {
  $config = config('pantheon_config_deploy.settings');
  $config_path = $config->get('writeable_config');
  $web_root = $config->get('webroot') . '/';
  echo "\r\n\r\nChecking for writeable config directory in: [" . $web_root . $config_path . "]\r\n";

  if (file_exists($web_root . $config_path)) {
    echo "FOUND\r\n";
    echo "Checking for obscured config directory...\r\n";
    $scan_dir = scandir($web_root . $config_path);
    $count = 0;
    foreach ($scan_dir as $dir_file) {
      if (strpos($dir_file, 'config_') === 0) {
        $count++;
        $files = scandir($web_root . $config_path . '/' . $dir_file);
        $files = count($files) - 2;
        echo "FOUND [" . $dir_file . "] with $files config files.\r\n";
      }
    }

    // Too many config directories
    if ($count > 1) {
      $state = 0;
      echo "Multiple config directories exist, only one can exist to turn on writeable mode. Run pc-remove-config to delete them.\r\n";
    }

    // No config directories
    if ($count == 0) {
      $state = 0;
      echo "There is no useable writeable config directory. Run pantheon pc-clone-config to create one.\r\n";
    }

    if ($state) {
      echo "Setting writeable config to ON\r\n";
      $file = fopen($web_root . $config_path . '/temp_active.true', 'w');
      fwrite($file, '');
      fclose($file);
    }
    else {
      echo "Setting writeable config to OFF\r\n";
      if (file_exists($web_root . $config_path . '/temp_active.true')) {
        unlink($web_root . $config_path . '/temp_active.true');
      }
    }
  }
  else {
    return "Writeable config directory doesn't exist. Run pc-clone-config to create one.\r\n";
  }
  return "Writeable config mode is " . ($state ? "ON" : "OFF") . "\r\n";
}

/**
 * Removes confi and sets writeable mode to OFF
 */
function drush_pantheon_config_deploy_drush_remove_config() {
  $config = config('pantheon_config_deploy.settings');
  $config_path = $config->get('writeable_config');
  $web_root = $config->get('webroot') . '/';
  global $config_directories;

  if (!isset($config_directories['active']) || empty($config_directories['active'])) {
    return drush_set_error('Cannot read configuration settings');
  }
  echo "\r\n\r\nStarting...\r\n";
  echo "Writeable config base directory: [" . $web_root . $config_path . "]\r\n";

  // Remove old writeable config
  echo "Searching for writeable config directory...\r\n";
  $found = FALSE;
  $count = 0;
  $dir_count = 0;
  if (file_exists($web_root . $config_path)) {
    $scan_dir = scandir($web_root . $config_path);
    foreach ($scan_dir as $dir_file) {
      if (strpos($dir_file, 'config_') === 0) {
        $dir_count++;
        $config_scan = scandir($web_root . $config_path . '/' . $dir_file);
        echo "Found: [" . $web_root . $config_path . '/' . $dir_file . "] with " . (count($config_scan) - 2) . " files\r\n";
        echo "Deleting configuration files";
        foreach ($config_scan as $config_file) {
          if (strpos(strrev($config_file), 'nosj.') === 0 || $config_file == '.htaccess') {
            unlink($web_root . $config_path . '/' . $dir_file . '/' . $config_file);
            echo ".";
            $count++;
            $found = TRUE;
          }
        }
        echo "\r\nRemoving directory...\r\n";
        rmdir($web_root . $config_path . '/' . $dir_file);
      }
    }
  }
  if (!$found) {
    echo "...\r\nNone found.\r\n";
  }
  else {
    echo "\r\nDeleted $dir_count writeable configuration directories and $count configuration files.\r\n";
  }

  // Turn writeable mode OFF
  drush_pantheon_config_deploy_drush_wconfig(0);
}

/**
 * Clones config to a writeable directory via drush
 */
function drush_pantheon_config_deploy_drush_clone_config() {
  $config = config('pantheon_config_deploy.settings');
  $config_path = $config->get('writeable_config');
  $web_root = $config->get('webroot') . '/';
  global $config_directories;

  if (!isset($config_directories['active']) || empty($config_directories['active'])) {
    return drush_set_error('Cannot read configuration settings');
  }
  echo "\r\n\r\nStarting...\r\n";
  echo "Writeable config: [" . $web_root . $config_path . "]\r\n";
  echo "Active config: [" . $config_directories['active'] . "]\r\n";

  // Remove old writeable config
  echo "Checking for existing writeable config...\r\n";
  $found = FALSE;
  $count = 0;
  if (file_exists($web_root . $config_path)) {
    $scan_dir = scandir($web_root . $config_path);
    foreach ($scan_dir as $dir_file) {
      if (strpos($dir_file, 'config_') === 0) {
        $config_scan = scandir($web_root . $config_path . '/' . $dir_file);
        echo "Found: [" . $web_root . $config_path . '/' . $dir_file . "] with " . count($config_scan) - 2 . " files\r\n";
        $found = TRUE;
      }
    }
  }
  if (!$found) {
    echo "\r\nNone found, proceeding with config clone operation.\r\n";
  }
  else {
    return "\r\A writeable configuration directory already exists, run pc-remove-config to remove it before cloning a new one.\r\n";
  }

  // Create config files structure
  echo "Creating temporary config directory structure...\r\n";
  if (!file_exists($web_root . $config_path)) {
    mkdir($web_root . $config_path);
  }
  $chars = 'abcdefghijklmnopqrstuvwxyz0123456789';
  $config_dir = 'config_';
  for ($i=0; $i<16; $i++) {
    $config_dir .= $chars[rand(0, strlen($chars) - 1)];
  }
  mkdir($web_root . $config_path . '/' . $config_dir);

  echo "New temporary active config: [" . $web_root . $config_path . '/' . $config_dir . "]\r\n";
  // Copy configuration files
  echo "Identifying config directory and copying config files...\r\n";
  echo "Config source: [" . $config_directories['active'] . "]\r\n";
  $config_files = scandir($web_root . '/' . $config_directories['active']);
  $count = 0;
  echo "Copying";
  foreach ($config_files as $config_file) {
    if (strpos(strrev($config_file), 'nosj.') === 0) {
      copy($web_root . '/' . $config_directories['active'] . '/' . $config_file, $web_root . $config_path . '/' . $config_dir . '/' . $config_file);
      echo ".";
      $count++;
    }
  }
  echo "\r\nCopied $count config files.\r\n";
  return "Pantheon writeable configuration clone is complete.";
}