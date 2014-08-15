#PHLIPS - CLIPS extension for PHP

##Original repository
[http://phlips.sourceforge.net](http://phlips.sourceforge.net)

##Motivation
The current version of PHLIPS (0.5.1) unfortunately not compiled in new versions of PHP - releases 5.4.x and 5.5.x.
![](https://www.github.com/stopassola/phlips.git/README_images/PHLIPS_errors.png)    
I make little adjusts on source clips.c to working again.

##Install
PS.: this tutorial is based on [MAMP version 2.2](http://www.mamp.info) (acronym for MacOSX + Apache + MySQL + PHP), running on OS X 10.9 Mavericks.

###1. Download
```
git clone https://github.com/stopassola/phlips.git
cd phlips
```

###2. Prepare the build environment for a PHP extension
`/Applications/MAMP/bin/php/php5.5.3/bin/phpize`

More information in [http://php.net/manual/en/install.pecl.phpize.php](http://php.net/manual/en/install.pecl.phpize.php)

This command print something like:
```
Configuring for:
PHP Api Version:         20121113
Zend Module Api No:      20121212
Zend Extension Api No:   220121212
```

###3. Specify the installation of PHP
If you have multiple PHP versions installed, you may be able to specify for which installation you'd like to build by using the --with-php-config option during configuration:

`./configure --with-php-config=/Applications/MAMP/bin/php/php5.5.3/bin/php-config`

###4. Build module clips.so from source code
`sudo make`

PS.: tested on version 5.5.3 of PHP (running on MAMP 2.2).

###5. Copy the _clips.so_ module to MAMP repository:
`cp modules/clips.so /Applications/MAMP/bin/php/php5.5.3/lib/php/extensions/no-debug-non-zts-20121212/`

###6. Restart the webserver

###7. Check if extension was install
```php
<?php
phpinfo();
?>
```
You should view the clips extension:
![](https://www.github.com/stopassola/phlips.git/README_images/PHLIPS_phpini.png) 

###8. Test some _expert system_ code:
```php
<?php
clips_clear();
clips_set_strategy(LEX_STRATEGY);
clips_assert("(quesito 2)");
clips_run();
$facts = clips_get_fact_list();
print_r($facts);
?>
```