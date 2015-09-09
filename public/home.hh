<?hh
namespace HCPublic;

class HomePage extends \HC\Page {
	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Home',
				'scss' => [
					'main' => true,
				],
				'js' => [
					'extenders' => true,
					'main' => true,
					'bootstrap-functions' => true,
                    'dashboard' => true
				]
			],
			'body' => true,
			'footer' => true,
		],
		'authentication' => true,
	];

	public function init($GET = [], $POST = []) {

        $scheduledDowntime = <ul class="list-group" style="max-height: 600px; overflow-y: auto; overflow-x: hidden;"></ul>;
        $unscheduledDowntime = <ul class="list-group" style="max-height: 600px; overflow-y: auto; overflow-x: hidden;"></ul>;

        $db = new \HC\DB();
        $result = $db->query('SELECT
                                `I`.`dateCreated`,
                                `I`.`dateLastConfirmed`,
                                `I`.`dateClosed`,
                                `I`.`status`,
                                `I`.`domainID`,
                                `I`.`databaseID`,
                                `I`.`serverID`
                              FROM
                                `issues` `I`
                              WHERE
                                `I`.`auto` = 1 ORDER BY `I`.`status` ASC, `I`.`id` DESC;');
        if($result) {
            foreach($result as $row) {
                $class = 'list-group-item';
                $content = <small class="text-center" style="display: block;">

                           </small>;



                if($row['dateClosed']) {
                    $content->appendChild(<x:frag>{date('Y-m-d H:i:s', $row['dateCreated'])}</x:frag>);

                    $datetime1 = new \DateTime();
                    $datetime1->setTimestamp($row['dateCreated']);

                    $datetime2 = new \DateTime();
                    $datetime2->setTimestamp($row['dateClosed']);

                    $duration = $datetime1->diff($datetime2);

                    $content->appendChild(<x:frag> - {$duration->format('%h:%i:%s')}</x:frag>);
                } else {
                    $content->appendChild(<x:frag>{date('Y-m-d H:i:s', $row['dateCreated'])}</x:frag>);
                    if($row['dateCreated'] !== $row['dateLastConfirmed']) {
                        $content->appendChild(<x:frag> - {date('Y-m-d H:i:s', $row['dateLastConfirmed'])}</x:frag>);
                    }
                }

                $affected = '';
                if($row['domainID'] && $row['serverID']) {
                    $affected = 'D' . $row['domainID'] . ' S' . $row['serverID'];
                } else if($row['databaseID']) {
                    $affected = 'DB' . $row['databaseID'];
                } else {
                    $affected = 'D' . $row['domainID'];
                }

                $content->appendChild(<x:frag> - {$affected}</x:frag>);

                switch($row['status']) {
                    case 2:
                        $class .= ' list-group-item-warning';
                        break;
                    case 3:
                        $class .= ' list-group-item-success';
                        break;
                    default:
                        $class .= ' list-group-item-danger';
                        break;
                }

                $unscheduledDowntime->appendChild(<li class={$class}>{$content}</li>);
            }
        }

		$container = <div class="container">
            <div class="row">
                <h1>{SITE_NAME} - {$this->settings['views']['header']['pageName']}</h1>
                <div class="clearfix"></div>
            </div>
            <div class="row">
                <div class="col-md-4">
                    <h3 class="text-center">Domains</h3>
                    <div id="domainsUp" class="chart forceGPU noselect">
                        <div class="spinner">
                          <div class="rect1"></div>
                          <div class="rect2"></div>
                          <div class="rect3"></div>
                          <div class="rect4"></div>
                          <div class="rect5"></div>
                        </div>
                    </div>
                </div>
                <div class="col-md-4">
                    <h3 class="text-center">Servers</h3>
                    <div id="serversUp" class="chart forceGPU noselect">
                        <div class="spinner">
                          <div class="rect1"></div>
                          <div class="rect2"></div>
                          <div class="rect3"></div>
                          <div class="rect4"></div>
                          <div class="rect5"></div>
                        </div>
                    </div>
                </div>
                <div class="col-md-4">
                    <h3 class="text-center">Databases</h3>
                    <div id="databasesUp" class="chart forceGPU noselect">
                        <div class="spinner">
                          <div class="rect1"></div>
                          <div class="rect2"></div>
                          <div class="rect3"></div>
                          <div class="rect4"></div>
                          <div class="rect5"></div>
                        </div>
                    </div>
                </div>
                <div class="clearfix"></div>
            </div>
            <div class="row">
                <div class="row">
                    <select class="form-control" id="timeScale">
                        <option value="0">Hour</option>
                        <option value="1">Day</option>
                        <option value="2">Week</option>
                        <option value="3">Month</option>
                    </select>
                </div>
                <div class="row">
                    <div id="serverHistoryApplicationResponseTime" class="chart forceGPU noselect">
                        <div class="spinner">
                          <div class="rect1"></div>
                          <div class="rect2"></div>
                          <div class="rect3"></div>
                          <div class="rect4"></div>
                          <div class="rect5"></div>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-lg-12">
                        <div id="serverHistoryUsage" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-lg-6">
                        <div id="historyAvailability" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>

                    <div class="col-lg-6">
                        <div id="historyResponseTime" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-lg-4">
                        <div id="serverHistoryApplicationRPM" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>

                    <div class="col-lg-4">
                        <div id="serverHistoryApplicationQPM" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>

                    <div class="col-lg-4">
                        <div id="serverHistoryApplicationAVGTimeCPUBound" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="row">
                    <div class="col-lg-4">
                        <div id="serverHistoryIOWait" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>

                    <div class="col-lg-4">
                        <div id="serverHistoryTPS" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>

                    <div class="col-lg-4">
                        <div id="serverHistoryNetworkTraffic" class="chart forceGPU noselect">
                            <div class="spinner">
                              <div class="rect1"></div>
                              <div class="rect2"></div>
                              <div class="rect3"></div>
                              <div class="rect4"></div>
                              <div class="rect5"></div>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="row">
                    <div class="col-md-6">
                        <h4 class="text-center">Unscheduled Downtime</h4>
                        {$unscheduledDowntime}
                    </div>
                    <div class="col-md-6">
                        <h4 class="text-center">Scheduled Downtime</h4>
                        {$scheduledDowntime}
                    </div>
                </div>
            </div>
        </div>;

		// Add the row to the container, and render the body
		$this->body = $container;

		return 1;
	}
}
