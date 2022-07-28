<?php
/**
 * @file
 * Example use of before.php for Pantheon webhooks
 */

/**
 * Example before.php
 *
 * Before you sync code / deploy code this will
 * 1. Put you in maintenance mode, then
 * 2. Make a copy of the envs current config as writeable config, then
 * 3. See after.php
 */
$drush_commands = [
  'state-set' => [
    'maintenance_mode',
    '1'
  ],
  'pc-clone-config' => []
];

// Iterate over each drush command and execute it on the system.
foreach ($drush_commands as $sub_command => $args) {
    $command = "drush {$sub_command} " . implode(' ', $args);
    echo sprintf('Running %s...', $command);
    passthru($command);
}