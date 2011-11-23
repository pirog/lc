$Id: README.txt,v 1.1.2.1.2.2 2009/10/18 02:49:51 aland Exp $

== TaxiSelect ==

* http://drupal.org/project/taxiselect

-- SUMMARY --

A widget to assist users entering hierarchical data using an autocomplete
textfield on node add/edit forms.


-- REQUIREMENTS --

* Drupal 6.x
* Taxonomy


-- INSTALLING --

* Copy the 'taxiselect' folder to your sites/all/modules directory.
* Go to Administer > Site building > Modules. Enable the module.

Read more about installing modules at http://drupal.org/node/70151

-- CONFIGURING --

* Navigate to Administer > Content management > Taxonomy
* Click on 'edit vocabulary' link for the appropriate vocabulary.
* Under 'Content types' section check a content type.
* Under 'Settings' section make sure that 'Tags' is NOT selected.
* Save

* Navigate to Administer > Content management > Taxonomy > Taxiselect settings
* Under 'Content types to associate this widget with:' check a content type.
* Under 'Vocabulary level TaxiSelect settings', make sure you check 'Use TaxiSelect for the selected content types'
* Save

-- UPGRADING --

1. One of the most IMPORTANT things to do BEFORE you upgrade, is to backup your site's files and database. More info: http://drupal.org/node/22281
2. Disable actual module. To do so go to Administer > Site building > Modules. Disable the module.
3. Just overwrite (or replace) the older module folder with the newer version.
4. Enable the new module. To do so go to Administer > Site building > Modules. Enable the module.
5. Run the update script. To do so go to the following address: www.yourwebsite.com/update.php
Follow instructions on screen. You must be log in as an administrator (user #1) to do this step.

Read more about upgrading modules: http://drupal.org/node/250790

-- CONTACT --

Current maintainers:
* Alan D. - http://drupal.org/user/198838

This project has been sponsored by:

* Internet Bird Collection
  http://ibc.lynxeds.com

-- OTHER CONTRIBUTORS --

Thanks to the following users:

lelizondob for the initial Drupal 6.x-1.0 port
Onopoc for testing and the initial README.txt
