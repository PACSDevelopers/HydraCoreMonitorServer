<?hh
namespace HCPublic\Downloads;

class BackupsPage extends \HC\Page {

    protected $settings = [
        'authentication' => true
    ];

    public function init($GET = [], $POST = []) {
        if($_SESSION['user']->hasPermission('Backup')) {
            if(isset($GET['id'])) {
                $db = new \HC\DB();
                $backup = $db->query('SELECT `D`.`id`, `DB`.`id` as `backupID`, `DB`.`dateStarted` as `backupDate` FROM `database_backups` `DB` LEFT JOIN `databases` `D` ON (`D`.`id` = `DB`.`databaseID`) WHERE `DB`.`id` = ?;', [$GET['id']]);
                if($backup) {
                    $backup = $backup[0];
                    $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
                    if(isset($globalSettings['backups'])) {
                        $file = $globalSettings['backups']['archive'] . '/' . $GET['id'] . '.tar.xz';
                        if(is_file($file)) {
                            $this->setRendered(true);
                            $information = finfo_open(FILEINFO_MIME_TYPE);
                            header('Pragma: public');
                            header('Content-Type:' . finfo_file($information, $file));
                            header('Content-Length: ' . filesize($file));
                            header('ETag: '. md5_file($file) .'');
                            header('Cache-Control: max-age=' . strtotime('+1 year', time()));
                            header('Expires: ' . strtotime('+1 year', time()));
                            header('Last-Modified: ' . filemtime($file));
                            header('Content-Disposition: attachment; filename="' . $backup['id'] . '-' . $backup['backupID'] . '-' . $backup['backupDate']  . '.tar.xz' . '"');
                            readfile($file);
                            finfo_close($information);
                            return 1;
                        }
                    }
                }
            }
        } else {
            return 403;
        }

        return 404;
	}
}
