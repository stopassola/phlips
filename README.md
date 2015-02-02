#PHLIPS - CLIPS extension for PHP

##Original repository
[http://phlips.sourceforge.net](http://phlips.sourceforge.net)

##Motivation
The current version of PHLIPS (0.5.1) unfortunately not compiled in new versions of PHP - releases 5.4.x and 5.5.x.
![](https://github.com/stopassola/phlips/blob/master/README_images/PHLIPS_errors.png)    
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
***
####Error: autoconf utility not available
```
Configuring for:
PHP Api Version:         20121113
Zend Module Api No:      20121212
Zend Extension Api No:   220121212
Cannot find autoconf. Please check your autoconf installation and the
$PHP_AUTOCONF environment variable. Then, rerun this script.
```
Install the autoconf in your **Linux** distro and repeat the last command:

`sudo apt-get install autoconf`

###3. Specify the installation of PHP
If you have multiple PHP versions installed, you may be able to specify for which installation you'd like to build by using the --with-php-config option during configuration:

`./configure --with-php-config=/Applications/MAMP/bin/php/php5.5.3/bin/php-config`

***
####Error: C compiler is not available 
```
checking for gcc... no
configure: error: in `/home/zend/phlips':
configure: error: no acceptable C compiler found in $PATH
See `config.log' for more details.
```
Install compiler utilities on your **Linux** distro and repeat the last command:

`sudo apt-get install build-essential`

###4. Build module _clips.so_ from source code
`sudo make`

PS.: tested on version 5.5.3 of PHP (running on MAMP 2.2), PHP 5.5.10 (on MAMP 3.0.5) and PHP 5.5.7 (on ZendServer Enterprise 6.3.0 running in Debian GNU/Linux 6.0.9 "squeeze").

###5. Copy the _clips.so_ module to MAMP repository:
`cp modules/clips.so /Applications/MAMP/bin/php/php5.5.3/lib/php/extensions/no-debug-non-zts-20121212/`

###6. Load the CLIPS extension (clips.so) in php.ini (configuration file of PHP)
`extension=clips.so`

###7. Restart the webserver
`sudo /usr/local/zend/bin/apachectl restart`

(command above tested in ZendServer Enterprise 6.3.0 running in Debian GNU/Linux 6.0.9 "squeeze")

###8. Check if extension was install
```php
<?php
phpinfo();
?>
```
You should view the clips extension:
![](https://github.com/stopassola/phlips/blob/master/README_images/PHLIPS_phpini.png) 

###9. Test some _expert system_ code:
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