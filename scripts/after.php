<?php
/**
 * @file
 * Example use of after.php for Pantheon webhooks
 */

/**
 * Example after.php
 *
 * After you sync code / deploy code this will
 * 1. First look at before.php to see how this started
 * 2. Switch the site to using read/write config as active config / version controlled as staging
 * 3. Import config from "staging" (your incoming repo changes)
 * 4. Turn OFF writeable config mode and clean up
 * 3. Turn OFF maintenance mode
 */
$drush_commands = [
  'pc-wconfig' => [
    '1',
  ],
  'bcim' => [],
  'pc-wconfig' => [
    '0',
  ],
  'state-set' => [
    'maintenance_mode',
    '0'
  ],
];

// Iterate over each drush command and execute it on the system.
foreach ($drush_commands as $sub_command => $args) {
    $command = "drush {$sub_command} " . implode(' ', $args);
    echo sprintf('Running %s...', $command);
    passthru($command);
}