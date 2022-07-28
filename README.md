Pantheon Configuration
======================

Provides a small collection of drush utilities to deploy configuration changes
to Backdrop CMS sites on Pantheon with their configuration in version control.

Requirements
------------

- A drush installation compatible with Backdrop CMS (https://backdropcms.org/project/drush)
- A terminus installation on your local environment (https://pantheon.io/docs/terminus/install)

Installation
------------

- First, install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.
- Paste the code snippet found in settings.snippet.php into your
  settings.local.php file or equivalent, directly after the _last_ definition
  of the $config_directories variable.


Documentation
-------------

This is a developer module specifically for Backdrop CMS sites on Pantheon.
It installs a collection of drush utilities that can be used to temporarily 
configure a Backdrop CMS site on Pantheon to use the writeable file system
for configuration management.

To use this module:
 - Ensure your site's active configuration IS in your repo, and is NOT in 
   the writeable filesystem.
 - Visit the module's config page to save your config location settings at
   /admin/pantheon-config/settings
 - When local changes modify or create JSON config files, use a workflow
   similar to the one below to deploy those changes to ENV(s)

Basic workflow:

 - First, make config affecting changes on your local environment, commit, and push. Then...
 - __terminus drush SITE.ENV pcrc__ REMOVES any temporary config files from SITE.ENV
 - __terminus drush SITE.ENV pccc__ CLONES your pre-deploy active config into the temporary config directory
 - __terminus drush SITE.ENV state-set maintenance_mode 1__ Optionally turn Maintenance Mode ON while changes are imported
 - __terminus drush SITE.ENV pcwc 1__ TURN ON Writeable Configuration
 - __terminus upstream:updates:apply SITE.ENV --updatedb --accept-upstream__ ONLY IF you're using an upstream, or just push like normal //
 - __git push origin [master / multidev env]__ ONLY IF you're not using an upstream, otherwise use the option above ^^
 - __terminus drush SITE.ENV bcim__ IMPORT configuration changes in the repo
 - __terminus drush SITE.ENV pcrc__ TURN OFF writeable config and cleanup the files
 - __terminus drush SITE.ENV state-set maintenance_mode 0__ Maintenance mode OFF assuming you turned it on

Issues
------

Bugs and Feature requests should be reported in the Issue Queue: https://github.com/backdrop-contrib/pantheon_config_deploy/issues


Current Maintainers
-------------------

- [Eric Toupin](https://github.com/eric2pin).
- Seeking additional maintainers.

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.