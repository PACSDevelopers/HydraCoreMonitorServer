<?hh

class FooterView extends \HC\View {

    public function init($settings = []) {
        $serverSide = <x:frag></x:frag>;
        $clientSide = <x:frag></x:frag>;
        $environment = <x:frag></x:frag>;

        if (ENVIRONMENT !== 'PRODUCTION') {
            $siteObject = $GLOBALS['HC_CORE']->getSite();
            $endTime = microtime(true);
            $startTime = $siteObject->getStartTime();
            $total_time = ($endTime - $startTime);
            $meetsTarget = true;

            if($total_time > 200) {
                $meetsTarget = false;
            }
            
            $total_time = number_format($total_time, 3);
            
            if($total_time == 1) {
                $total_time .= ' seconds';
            } else {
                $total_time .= ' seconds';
            }
            
            $environment = getenv('SERVERNAME');
            if(!$environment) {
                $environment = '';
            }
            
            $environment = <div class="col-xs-12">{$environment}</div>;
            $serverSide = <div class="col-xs-6"></div>;
            $serverSide->appendChild(<small>Page generated in {$total_time}.</small>);
            $serverSide->appendChild(<br />);
            $serverSide->appendChild(<small>CPU: {$siteObject->getCPUUsage(true) . '% / ' . $siteObject->getTotalCPUUsage(true) . '% - ' . $siteObject->getTimeCPUBound(true)}%</small>);
            $serverSide->appendChild(<br />);
            $serverSide->appendChild(<small>Memory: {$siteObject->getPeakMemoryUsage(true, true) . ' / ' . $siteObject->getScriptMemoryLimit(true, true) . ' / ' . $siteObject->getServerMemoryLimit(true)}</small>);
            $serverSide->appendChild(<br />);
            $serverSide->appendChild(<small>Queries: {$siteObject->getNumberOfNonSelects() .  ' - '  . $siteObject->getNumberOfSelects() . ' / ' . $siteObject->getNumberOfCacheHits() .  ' (' . $siteObject->getCacheEfficiency(true) . '%'})</small>);
            
            $clientSide = <div class="col-xs-6"></div>;
            $clientSide->appendChild(<small id="pageLoadedTime"></small>);
            $clientSide->appendChild(<br />);
            $clientSide->appendChild(<small id="pageResponseTime"></small>);
            $clientSide->appendChild(<br />);
            $clientSide->appendChild(<small id="pageResources"></small>);
            $clientSide->appendChild(<br />);
            $clientSide->appendChild(<small id="pageMemory"></small>);
        }

        $footer = <footer class="container-fluid text-center">
            <div class="row">
              <div id="devStats">
                {$serverSide}
                {$clientSide}
                {$environment}
              </div>
              <div class="col-xs-12">
                  <small>Copyright © {AUTHOR} - {date('Y')}</small>
              </div>
            </div>
        </footer>;

        return $footer;
    }

}
