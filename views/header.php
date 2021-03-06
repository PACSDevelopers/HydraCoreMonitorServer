<?hh


class HeaderView extends \HC\View {

    public function init($settings = []) {
        if ((isset($settings['pageName'])) && (!empty($settings['pageName']))) {

            $pageName = SITE_NAME . ' - ' . $settings['pageName'];

        } else {

            $pageName = SITE_NAME;

        }
        
        $onLoad = '';

        if (ENVIRONMENT !== 'PRODUCTION') {
            $onLoad .= 'drawHCStats();';
        }
        
        if(isset($viewSettings['onLoad'])){
          $onLoad .= $viewSettings['onLoad'];
        }

        $header = <x:frag>
                        <meta charset="utf-8" />
                        <title>{$pageName}</title>
                        <base href={PROTOCOL . '://' . SITE_DOMAIN . '/'}  />

                        <link rel="shortcut icon" href={PROTOCOL . '://' . SITE_DOMAIN . '/favicon.ico'} type="image/icon" />
                        <link rel="icon" href={PROTOCOL . '://' . SITE_DOMAIN . '/favicon.ico'} type="image/icon" />
            
                        <meta name="viewport" content="width=device-width, height=device-height, initial-scale=1, user-scalable=no, target-densitydpi=device-dpi" />
            
                        <meta property="og:title" content={$pageName} />
                        <meta property="og:type" content="website" />
                        <meta property="og:url" content={PROTOCOL . '://' . SITE_DOMAIN} />
                        <meta property="og:image" content={PROTOCOL . '://' . SITE_DOMAIN . '/favicon.png'} />
        
                        <script type="text/javascript" src="https://www.google.com/jsapi"></script>
                        <script type="text/javascript">
                            {'google.load(\'visualization\', \'1.0\', {\'packages\':[\'corechart\']}); google.setOnLoadCallback(function(){$(document).trigger(\'chartDraw\');});'}
                        </script>
        
                        {HC\Page::generateComponents(['css' => ['/resources/css/bootstrap-theme.css']], 'css')}
                        {HC\Page::generateResources($settings, 'css')}
                        {HC\Page::generateComponents([], 'js')}
                        {HC\Page::generateResources($settings, 'js')}
                        <script type="text/javascript">{'window.onload = function(){' . $onLoad . '};'}</script>
                    </x:frag>;
        return $header;

    }

}
