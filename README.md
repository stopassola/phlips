#PHLIPS - CLIPS extension for PHP

##Original repository
[http://phlips.sourceforge.net](http://phlips.sourceforge.net)

##Motivation
The current version of PHLIPS (0.5.1) unfortunately not compiled in new versions of PHP - releases 5.4.x, 5.5.x and 5.6.x.
![](https://github.com/stopassola/phlips/blob/master/README_images/PHLIPS_errors.png)    
I make little adjusts on source clips.c to working again.

##Install
PS.: this tutorial is based on [MAMP version 3.4](http://www.mamp.info) (acronym for Mac + Apache + MySQL + PHP), running on OS X 10.11 El Capitan.

PS.: I tested on version 5.5.3 of PHP (running on MAMP 2.2 running on OS X 10.9 Mavericks), PHP 5.5.10 (on MAMP 3.0.5 on OSX 10.10 Yosemite) and PHP 5.5.7 (on ZendServer Enterprise 6.3.0 running in Debian GNU/Linux 6.0.9 "squeeze").

###1. Download
```
git clone https://github.com/stopassola/phlips.git
cd phlips
```

###2. Prepare the build environment for a PHP extension
`/Applications/MAMP/bin/php/php5.6.10/bin/phpize`

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

#####2.1 Linux

Install the autoconf in your **Linux** distro:

`sudo apt-get install autoconf`

#####2.2 OS X

Open **Terminal** and execute the follow commands:

```
cd ..
curl http://ftp.gnu.org/gnu/autoconf/autoconf-latest.tar.gz > autoconf.tar.gz
tar -xzf autoconf.tar.gz
cd autoconf-x.yy
./configure --prefix=/usr/local/
make
sudo make install
export PHP_AUTOCONF=/usr/local/bin/autoconf
```
PS.: the `autoconf-x.yy` shoud be replaced by current version of autoconf. In my case: `autoconf-2.69`

#####2.3 Repeat the last command

```
cd ../phlips/
/Applications/MAMP/bin/php/php5.6.10/bin/phpize
```

If you don't enter in `phplips` directory (where fonts exist), the follow message appear:

```
Cannot find config.m4. 
Make sure that you run '/Applications/MAMP/bin/php/php5.6.10/bin/phpize' in the top level source directory of the module
```

#####2.4 Fatal error: 'php.h' file not found

![](https://github.com/stopassola/phlips/blob/master/README_images/PHLIPS_phpize_error.png)

If you see the follow message, it's because your PHP instalation not have the source code of PHP (current release):

![](https://github.com/stopassola/phlips/blob/master/README_images/PHLIPS_phpize_error.png)

```
cd /Applications/MAMP/bin/php/php5.6.10/
mkdir include
cd include
```

Download the PHP source code from your specific instalation:
<http://www.php.net/releases/>

```
cd ~/Downloads/
tar -xzf php-5.6.10.tar.gz
mv php-5.6.10 /Applications/MAMP/bin/php/php5.6.10/include/php
cd /Applications/MAMP/bin/php/php5.6.10/include/php
sudo ./configure --without-iconv
```

PS.: the parameter `--without-iconv` is important, otherwise some file is not available and errors is generated:

```
/Applications/MAMP/bin/php/php5.6.10/include/php/Zend/zend.h:51:11: fatal error:
      'zend_config.h' file not found
\# include <zend_config.h>
          ^
1 error generated.
```

and

```
/Applications/MAMP/bin/php/php5.6.10/include/php/Zend/../TSRM/TSRM.h:20:11: fatal error: 'tsrm_config.h' file not found
\# include <tsrm_config.h>
          ^
1 error generated.
```

![](https://github.com/stopassola/phlips/blob/master/README_images/PHLIPS_PHP_install.png)

###3. Specify the installation of PHP
If you have multiple PHP versions installed, you may be able to specify for which installation you'd like to build by using the --with-php-config option during configuration:

```
cd phlips_source_directory
./configure --with-php-config=/Applications/MAMP/bin/php/php5.6.10/bin/php-config`
```

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

In **OSX** make sure to install the Xcode with command line:
`xcode-select --install`

###4. Build module _clips.so_ from source code
`sudo make`

###5. Copy the _clips.so_ module to MAMP repository:
`cp modules/clips.so /Applications/MAMP/bin/php/php5.6.10/lib/php/extensions/no-debug-non-zts-20131226/`

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