<?hh

class BodyView extends \HC\View {

    public function init($settings = [], $body = '') {
        $navbar = <x:frag></x:frag>;
        $navButton = <x:frag></x:frag>;
        if(isset($_SESSION['user'])) {
            $navItems = <ul class="nav navbar-nav">
                            <li role="listitem"> <a href={PROTOCOL.'://'.SITE_DOMAIN . '/home'}>Home</a> </li>
                            <li role="listitem"> <a href={PROTOCOL.'://'.SITE_DOMAIN . '/domains'}>Domains</a> </li>
                            <li role="listitem"> <a href={PROTOCOL.'://'.SITE_DOMAIN . '/servers'}>Servers</a> </li>
                            <li role="listitem"> <a href={PROTOCOL.'://'.SITE_DOMAIN . '/databases'}>Databases</a> </li>
                            <li role="listitem"> <a href={PROTOCOL.'://'.SITE_DOMAIN . '/settings'}>Settings</a> </li>
                            <li role="listitem"> <a href={PROTOCOL.'://'.SITE_DOMAIN . '/logout'}>Logout</a> </li>
                        </ul>;
              
            $navbar->appendChild($navItems);
              
            $navButton = <button class="navbar-toggle" type="button" data-toggle="collapse" data-target="#navbar-main">
                            <span class="icon-bar"></span>
                            <span class="icon-bar"></span>
                            <span class="icon-bar"></span>
                         </button>;
        }
        
        $headerButtonsRight = <x:frag></x:frag>;
        if(isset($settings['headerButtonsRight'])) {
          foreach($settings['headerButtonsRight'] as $button) {
              $headerButtonsRight->appendChild($button);
          }
        }
        
        $headerButtonsLeft = <x:frag></x:frag>;
        if(isset($settings['headerButtonsLeft'])) {
          foreach($settings['headerButtonsLeft'] as $button) {
              $headerButtonsLeft->appendChild($button);
          }
        }
        
        $body = <x:frag>
                <header>
                    <nav class="navbar navbar-default navbar-fixed-top" role="navigation">
                        <div class="container">
                            <div class="navbar-header">
                                <a class="navbar-brand" href={PROTOCOL.'://'.SITE_DOMAIN}><img src={PROTOCOL.'://'.SITE_DOMAIN . '/resources/images/logo.png'} alt="HydraCore" id="mainLogo" width="186" height="50" /></a>
                                {$navButton}
                            </div>
                            <div class="navbar-collapse collapse" id="navbar-main" role="list">
                                {$navbar}
                            </div>
                        </div>
                    </nav>
                </header>
                <div class="clearfix"></div>
                <section id="page">
                    <div class="row col-lg-12">
                        <div class="headerButtons">
                            <div class="pull-left col-sm-3">
                              {$headerButtonsLeft}
                            </div>
                            <div class="pull-right col-sm-3">
                              {$headerButtonsRight}
                            </div>
                            <div class="clearfix"></div>
                        </div>
                        <div class="clearfix"></div>
                    </div>
                    {$body}
                </section>
              </x:frag>;
        
      return $body;
    }
}
