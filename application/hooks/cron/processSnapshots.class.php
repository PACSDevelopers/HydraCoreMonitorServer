<?hh


  namespace HCMS\Hooks\Cron;

  /**
   * Class ProcessSnapshots
   * @package HC\Hooks\Cron
   */

  class ProcessSnapshots extends \HC\Hooks\Cron

  {
      protected $settings = [];

      public function __construct($settings = [])

      {
          $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
          if(isset($globalSettings['gcloud'])) {
              $this->settings = $this->parseOptions($this->settings, $globalSettings['gcloud']);
          }

          if(is_array($settings)) {
              $this->settings = $settings = $this->parseOptions($this->settings, $settings);
          }
      }

      public function run()

      {
          echo 'Processing Snapshots' . PHP_EOL;

          $db = new \HC\DB();
          $result = $db->read('disks', [], ['status' => 1]);

          // Authenticate with GCE Compute
          $credentials = new \Google_Auth_AssertionCredentials(
              $this->settings['clientEmail'],
              ['https://www.googleapis.com/auth/compute'],
              file_get_contents($this->settings['privateKey'])
          );

          $client = new \Google_Client();
          $client->setAssertionCredentials($credentials);
          if ($client->getAuth()->isAccessTokenExpired()) {
              $client->getAuth()->refreshTokenWithAssertion();
          }

          $compute = new \Google_Service_Compute($client);

          // Create snapshots of snapshottable disks
          $date = date('d-m-Y');
          $time = time();
          foreach ($result as $row) {
              $checkTime = ($time - $row['snapshotInterval'] * 3600);

              if ($checkTime >= $row['lastSnapshot']) {
                  $disk = $row['title'];
                  $zone = $row['zone'];

                  $snapshot = new \Google_Service_Compute_Snapshot();
                  $snapshot->setName($disk . '-' . $date);

                  try {
                      $compute->disks->createSnapshot('pacs-tools-red', $zone, $disk, $snapshot);
                      $db->update('disks', ['id' => $row['id']], ['lastSnapshot' => $time]);
                      echo 'Snapshotting Disk ' . $disk . PHP_EOL;
                  } catch (\Exception $e) {
                      throw new \Exception('Could not create snapshot of disk ' . $disk . ': ' . $e->getMessage());
                      return false;
                  }
              }
          }

          echo 'Processed Snapshots' . PHP_EOL;
          return true;
      }
  }
