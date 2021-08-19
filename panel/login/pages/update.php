<?php
    function copyDir($src, $dst) {
        $dir = opendir($src);
        // @mkdir($dst);
        while(false !== ($file = readdir($dir))) {
            if (($file != '.') && ($file != '..')) {
                if (is_dir($src . '/' . $file)) { 
                    copyDir($src . '/' . $file,$dst . '/' . $file);
                }
                else {
                    copy($src . '/' . $file,$dst . '/' . $file);
                }
            }
        }
        closedir($dir); 
    }

    system("rm -rf /tmp/artemis");
    system("git clone git@github.com:crimist/Artemis.git /tmp/artemis");
    copyDir("/tmp/Artemis/panel/", "../../"); // ../../ = (pages/login) -> html/artemis

    echo "Updated!";
?>