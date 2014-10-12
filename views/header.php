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
        
                        <link rel="stylesheet" type="text/css" href={PROTOCOL.'://'.SITE_DOMAIN . '/components/bootstrap/css/bootstrap.css'} />
                        <link rel="stylesheet" type="text/css" href={PROTOCOL.'://'.SITE_DOMAIN . '/components/bootstrap/css/bootstrap-theme.css'} />
                        <link rel="stylesheet" type="text/css" href={PROTOCOL.'://'.SITE_DOMAIN . '/components/font-awesome/css/font-awesome.css'} />
        
                        <script type="text/javascript" src={PROTOCOL.'://'.SITE_DOMAIN . '/components/jquery/jquery-1.11.1.min.js'}></script>
                        <script type="text/javascript" src={PROTOCOL.'://'.SITE_DOMAIN . '/components/modernizr/modernizr.min.js'}></script>
                        <script type="text/javascript" src={PROTOCOL.'://'.SITE_DOMAIN . '/components/webshim/polyfiller.min.js'}></script>
                        <script type="text/javascript" src={PROTOCOL.'://'.SITE_DOMAIN . '/components/bootstrap/js/bootstrap.js'}></script>
                        <script type="text/javascript">{'window.onload = function(){' . $onLoad . '};'}</script>
                        
                        {POTENTIAL_XSS_HOLE(HC\Page::generateResources($settings))}
                    </x:frag>;
        
        return $header;

    }

}
