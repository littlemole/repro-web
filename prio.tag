<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>controller.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ctrl/</path>
    <filename>controller_8h.html</filename>
    <class kind="class">reproweb::WebApplicationContext</class>
    <member kind="function">
      <type>router&lt; F &gt;</type>
      <name>route</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gaac161bdfc52ed1ce5e7d22bd043501db</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F fun)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>GET</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gaef23d8afb7170f8d42d798dbab327d06</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>POST</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gaac05d4e569bbf3f832bfb13aa00cf4a9</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>DEL</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gab9144873d8e9fdbc3d1f059a07c6b22e</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>PUT</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga4b01197386573aa8d48c752ba034ae54</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>filter_router&lt; F &gt;</type>
      <name>filter</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gabc3c899d9b551e6292b99f1fa36a963f</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F f, int priority=0)</arglist>
    </member>
    <member kind="function">
      <type>completion_filter_router&lt; F &gt;</type>
      <name>completion_filter</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gabbad223182df93ef4358941ef9349f11</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F f, int priority=0)</arglist>
    </member>
    <member kind="function">
      <type>flush_filter_router&lt; F &gt;</type>
      <name>flush_filter</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga5415e1fdb035f3945730593e337192b3</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F f, int priority=0)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>render_error</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga19666b4f4490393dcd8057b86233a2a9</anchor>
      <arglist>(prio::Response &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>redirect</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga6dbba79ffb7be53f096189a323e0beca</anchor>
      <arglist>(prio::Response &amp;res, Args ... args)</arglist>
    </member>
    <member kind="function">
      <type>exception_handler&lt; F &gt;</type>
      <name>ex_handler</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga2caa64e5bccb8fa70b6e70db590688ee</anchor>
      <arglist>(F fun)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>filter.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ctrl/</path>
    <filename>filter_8h.html</filename>
    <class kind="class">reproweb::FilterChain</class>
  </compound>
  <compound kind="file">
    <name>front_controller.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ctrl/</path>
    <filename>front__controller_8h.html</filename>
    <class kind="class">reproweb::FrontController</class>
    <class kind="class">reproweb::static_content</class>
    <member kind="function">
      <type>std::shared_ptr&lt; diy::Context &gt;</type>
      <name>ctx</name>
      <anchorfile>front__controller_8h.html</anchorfile>
      <anchor>aab995fcffe851b308739f80e213eb310</anchor>
      <arglist>(prio::Request &amp;req)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>handler.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ctrl/</path>
    <filename>handler_8h.html</filename>
    <member kind="typedef">
      <type>std::function&lt; void(prio::Request &amp;req, prio::Response &amp;res)&gt;</type>
      <name>http_handler_t</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga2cb9980c09579508bb89e1330b613c15</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::function&lt; void(prio::Request &amp;req, prio::Response &amp;res, std::shared_ptr&lt; FilterChain &gt; chain)&gt;</type>
      <name>http_filter_t</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gae47f9bc633eaf551c9f65633e8cf98f5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>redissession.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ctrl/</path>
    <filename>redissession_8h.html</filename>
    <includes id="session_8h" name="session.h" local="yes" imported="no">reproweb/ctrl/session.h</includes>
    <class kind="class">reproweb::RedisSessionProvider</class>
  </compound>
  <compound kind="file">
    <name>session.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ctrl/</path>
    <filename>session_8h.html</filename>
    <includes id="controller_8h" name="controller.h" local="yes" imported="no">reproweb/ctrl/controller.h</includes>
    <includes id="filter_8h" name="filter.h" local="yes" imported="no">reproweb/ctrl/filter.h</includes>
    <includes id="json_2json_8h" name="json.h" local="yes" imported="no">reproweb/json/json.h</includes>
    <class kind="class">reproweb::Session</class>
    <class kind="class">reproweb::SessionProvider</class>
    <class kind="class">reproweb::MemorySessionProvider</class>
    <class kind="class">reproweb::SessionFilter</class>
    <class kind="struct">reproweb::session_filter</class>
    <member kind="function">
      <type>std::shared_ptr&lt; Session &gt;</type>
      <name>req_session</name>
      <anchorfile>session_8h.html</anchorfile>
      <anchor>af107ee12dc3c56844273fd52b9ce256d</anchor>
      <arglist>(prio::Request &amp;req)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>is_authenticated</name>
      <anchorfile>session_8h.html</anchorfile>
      <anchor>a30beade8753c2efae7d0f0ba6f272566</anchor>
      <arglist>(prio::Request &amp;req)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>invalidate_session</name>
      <anchorfile>session_8h.html</anchorfile>
      <anchor>aada42a944c1906cfa4d66047f5cf0069</anchor>
      <arglist>(prio::Request &amp;req)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>ssi.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ctrl/</path>
    <filename>ssi_8h.html</filename>
    <includes id="front__controller_8h" name="front_controller.h" local="yes" imported="no">reproweb/ctrl/front_controller.h</includes>
    <class kind="class">reproweb::SSIResolver</class>
    <class kind="class">reproweb::ssi_content</class>
  </compound>
  <compound kind="file">
    <name>json.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/json/</path>
    <filename>json_2json_8h.html</filename>
    <includes id="json_2json_8h" name="json.h" local="no" imported="no">json/json.h</includes>
    <class kind="struct">reproweb::subscription</class>
    <class kind="class">reproweb::EventBus</class>
    <member kind="function">
      <type>repro::Future</type>
      <name>forEach</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga09241712ea12f24c4ed37b03a2b7cdf4</anchor>
      <arglist>(Json::Value json, F f)</arglist>
    </member>
    <member kind="function">
      <type>Json::Value</type>
      <name>sort</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga13a4de05719b7d00a37a39878ebe9af3</anchor>
      <arglist>(const Json::Value &amp;arrayIn, const std::string member)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>jwt.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/json/</path>
    <filename>jwt_8h.html</filename>
    <includes id="json_2json_8h" name="json.h" local="yes" imported="no">reproweb/json/json.h</includes>
    <class kind="class">reproweb::JWT</class>
    <member kind="function">
      <type>void</type>
      <name>json_response</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga0668dd651539527c033e05e645473a14</anchor>
      <arglist>(prio::Response &amp;res, Json::Value json)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>json_err</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga50d38a00c8791085c9dd1224394873b6</anchor>
      <arglist>(prio::Response &amp;res)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>config.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/tools/</path>
    <filename>config_8h.html</filename>
    <includes id="json_2json_8h" name="json.h" local="yes" imported="no">reproweb/json/json.h</includes>
    <class kind="class">reproweb::Config</class>
  </compound>
  <compound kind="file">
    <name>ic.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/tools/</path>
    <filename>ic_8h.html</filename>
    <class kind="class">reproweb::miconv</class>
  </compound>
  <compound kind="file">
    <name>i18n.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/view/</path>
    <filename>i18n_8h.html</filename>
    <class kind="class">reproweb::I18N</class>
    <class kind="class">reproweb::i18n_props</class>
  </compound>
  <compound kind="file">
    <name>tpl.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/view/</path>
    <filename>tpl_8h.html</filename>
    <includes id="json_2json_8h" name="json.h" local="yes" imported="no">reproweb/json/json.h</includes>
    <includes id="i18n_8h" name="i18n.h" local="yes" imported="no">reproweb/view/i18n.h</includes>
    <includes id="ssi_8h" name="ssi.h" local="yes" imported="no">reproweb/ctrl/ssi.h</includes>
    <class kind="class">reproweb::mustache</class>
    <class kind="class">reproweb::TplStore</class>
    <class kind="class">reproweb::view_templates</class>
    <class kind="class">reproweb::AbstractView</class>
    <class kind="class">reproweb::TemplateView</class>
    <class kind="class">reproweb::ViewI18nDecorator</class>
    <class kind="class">reproweb::ViewMustacheDecorator</class>
    <class kind="class">reproweb::ViewSSIDecorator</class>
    <class kind="class">reproweb::I18nSSIMustacheView</class>
    <class kind="class">reproweb::I18nMustacheView</class>
    <class kind="class">reproweb::MustacheView</class>
    <class kind="class">reproweb::SSIMustacheView</class>
  </compound>
  <compound kind="file">
    <name>web_webserver.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/</path>
    <filename>web__webserver_8h.html</filename>
    <includes id="front__controller_8h" name="front_controller.h" local="yes" imported="no">reproweb/ctrl/front_controller.h</includes>
    <includes id="session_8h" name="session.h" local="yes" imported="no">reproweb/ctrl/session.h</includes>
    <class kind="class">reproweb::WebServer</class>
  </compound>
  <compound kind="file">
    <name>sse.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ws/</path>
    <filename>sse_8h.html</filename>
    <class kind="class">reproweb::SSEConnection</class>
    <class kind="class">reproweb::sse_controller</class>
  </compound>
  <compound kind="file">
    <name>websocket.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ws/</path>
    <filename>websocket_8h.html</filename>
    <class kind="class">reproweb::WsConnection</class>
  </compound>
  <compound kind="file">
    <name>ws.h</name>
    <path>/home/mike/workspace/repro-web/include/reproweb/ws/</path>
    <filename>ws_8h.html</filename>
    <includes id="websocket_8h" name="websocket.h" local="yes" imported="no">reproweb/ws/websocket.h</includes>
    <class kind="class">reproweb::ws_controller</class>
  </compound>
  <compound kind="class">
    <name>reproweb::AbstractView</name>
    <filename>classreproweb_1_1AbstractView.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future</type>
      <name>render</name>
      <anchorfile>classreproweb_1_1AbstractView.html</anchorfile>
      <anchor>a21bc2d1a19453946978d33f401ece6a4</anchor>
      <arglist>(prio::Request &amp;req, prio::Response &amp;res, const std::string &amp;page, Json::Value value)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual repro::Future&lt; std::string &gt;</type>
      <name>render_content</name>
      <anchorfile>classreproweb_1_1AbstractView.html</anchorfile>
      <anchor>a2887b47198c83594f2fab4916efae7ab</anchor>
      <arglist>(prio::Request &amp;req, const std::string &amp;page, Json::Value value)=0</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>redirect</name>
      <anchorfile>classreproweb_1_1AbstractView.html</anchorfile>
      <anchor>aeea775759562cd400f32336b698a75de</anchor>
      <arglist>(prio::Response &amp;res, const std::string &amp;url)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>redirect</name>
      <anchorfile>classreproweb_1_1AbstractView.html</anchorfile>
      <anchor>aaf334933153fef018f71c4a3df2bdf72</anchor>
      <arglist>(prio::Request &amp;req, prio::Response &amp;res, const std::string &amp;url)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>flush_content</name>
      <anchorfile>classreproweb_1_1AbstractView.html</anchorfile>
      <anchor>a80f784e1a83c2b4f364f36f365d7ffa0</anchor>
      <arglist>(prio::Response &amp;res, const std::string &amp;content)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>render_error</name>
      <anchorfile>classreproweb_1_1AbstractView.html</anchorfile>
      <anchor>a5062c84314bf867f2928590175c4a569</anchor>
      <arglist>(prio::Response &amp;res, const std::exception &amp;ex)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::string</type>
      <name>get_locale</name>
      <anchorfile>classreproweb_1_1AbstractView.html</anchorfile>
      <anchor>ade517109374ae8f1124be258f20495f1</anchor>
      <arglist>(prio::Request &amp;req)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::Config</name>
    <filename>classreproweb_1_1Config.html</filename>
    <member kind="function">
      <type></type>
      <name>Config</name>
      <anchorfile>classreproweb_1_1Config.html</anchorfile>
      <anchor>a81551900c968f420907cd766394855eb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Config</name>
      <anchorfile>classreproweb_1_1Config.html</anchorfile>
      <anchor>afdf4af6ef20e77a1af59c8c8120cf0d4</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>Config &amp;</type>
      <name>load</name>
      <anchorfile>classreproweb_1_1Config.html</anchorfile>
      <anchor>a9b2ca82643b8929a9d1364cc638d7d87</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>Json::Value &amp;</type>
      <name>json</name>
      <anchorfile>classreproweb_1_1Config.html</anchorfile>
      <anchor>a09bba46a232916c91553176a494bd0e2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Json::Value &amp;</type>
      <name>get</name>
      <anchorfile>classreproweb_1_1Config.html</anchorfile>
      <anchor>a3c89b2ea611e14c78fb0be79415e9079</anchor>
      <arglist>(const std::string &amp;key)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getString</name>
      <anchorfile>classreproweb_1_1Config.html</anchorfile>
      <anchor>af4468fc0076a997f0f18a5ba6432b113</anchor>
      <arglist>(const std::string &amp;key)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getInt</name>
      <anchorfile>classreproweb_1_1Config.html</anchorfile>
      <anchor>a7327c015ec7c1549e1fe3c9a40be8931</anchor>
      <arglist>(const std::string &amp;key)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::EventBus</name>
    <filename>classreproweb_1_1EventBus.html</filename>
    <member kind="function">
      <type>std::string</type>
      <name>subscribe</name>
      <anchorfile>classreproweb_1_1EventBus.html</anchorfile>
      <anchor>a07181cfa301fa5e0ccf5b1ec26f0a58c</anchor>
      <arglist>(const std::string &amp;topic, std::function&lt; void(Json::Value)&gt; observer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unsubscribe</name>
      <anchorfile>classreproweb_1_1EventBus.html</anchorfile>
      <anchor>a9547755940c5c476a0793e0f112a5b78</anchor>
      <arglist>(const std::string &amp;topic, const std::string &amp;id)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>notify</name>
      <anchorfile>classreproweb_1_1EventBus.html</anchorfile>
      <anchor>aad62ea11c149e5f87c5400b3c6e75ec1</anchor>
      <arglist>(const std::string &amp;topic, Json::Value value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classreproweb_1_1EventBus.html</anchorfile>
      <anchor>a5b81534f40c4956d049858a2de992ce6</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::FilterChain</name>
    <filename>classreproweb_1_1FilterChain.html</filename>
    <member kind="function">
      <type>void</type>
      <name>next</name>
      <anchorfile>classreproweb_1_1FilterChain.html</anchorfile>
      <anchor>a4005ec230c46e8a96fc8a7410d22f030</anchor>
      <arglist>(prio::Request &amp;req, prio::Response &amp;res)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::FrontController</name>
    <filename>classreproweb_1_1FrontController.html</filename>
    <member kind="function">
      <type></type>
      <name>FrontController</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>aad2991b194078eb10b83832b963c7a6e</anchor>
      <arglist>(std::shared_ptr&lt; diy::Context &gt; ctx)</arglist>
    </member>
    <member kind="function">
      <type>FrontController &amp;</type>
      <name>registerHandler</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a553b23d976d92717cd119431b1e7b725</anchor>
      <arglist>(const std::string &amp;method, const std::string &amp;path, http_handler_t handler)</arglist>
    </member>
    <member kind="function">
      <type>FrontController &amp;</type>
      <name>registerFilter</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>adce070ff1b99f8a1f0705c30c30200c6</anchor>
      <arglist>(const std::string &amp;method, const std::string &amp;path, http_filter_t filter, int prio=0)</arglist>
    </member>
    <member kind="function">
      <type>FrontController &amp;</type>
      <name>registerFlushFilter</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a210bff77375eb00bbc6040266e688530</anchor>
      <arglist>(const std::string &amp;method, const std::string &amp;path, http_filter_t filter, int prio=0)</arglist>
    </member>
    <member kind="function">
      <type>FrontController &amp;</type>
      <name>registerCompletionFilter</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a2c39e4b7893fb04cc10e60ccc95663eb</anchor>
      <arglist>(const std::string &amp;method, const std::string &amp;path, http_filter_t filter, int prio=0)</arglist>
    </member>
    <member kind="function">
      <type>FrontController &amp;</type>
      <name>registerStaticHandler</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a66a15eb787d44b900b7959b5862e3bd3</anchor>
      <arglist>(const std::string &amp;method, const std::string &amp;path, http_handler_t handler)</arglist>
    </member>
    <member kind="function">
      <type>FrontController &amp;</type>
      <name>registerExceptionHandler</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a658c232883470d9089a83f875bd01745</anchor>
      <arglist>(typename ExceptionHandlerInfo&lt; E &gt;::ex_handler_t handler)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dispatch</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a914cb3ba2fa97abe11da6339c4882697</anchor>
      <arglist>(const std::string &amp;path, prio::Request &amp;req, prio::Response &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>repro::Future&lt; std::string &gt;</type>
      <name>include</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a1266bf28c8d476ac4182749fbe91d1ea</anchor>
      <arglist>(const prio::Request &amp;req, const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>handle_exception</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a9491ade3281d919806a9382d1e839850</anchor>
      <arglist>(const std::exception &amp;ex, prio::Request &amp;req, prio::Response &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>handle_ex</name>
      <anchorfile>classreproweb_1_1FrontController.html</anchorfile>
      <anchor>a48c4edd46b556aebd730374bd7b2e5a1</anchor>
      <arglist>(prio::Request &amp;req, prio::Response &amp;res)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::I18N</name>
    <filename>classreproweb_1_1I18N.html</filename>
    <member kind="function">
      <type></type>
      <name>I18N</name>
      <anchorfile>classreproweb_1_1I18N.html</anchorfile>
      <anchor>a11d01e22b3e5db9d247853bbf0199f93</anchor>
      <arglist>(const std::string &amp;base, const std::vector&lt; std::string &gt; &amp;locales)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>find_locale</name>
      <anchorfile>classreproweb_1_1I18N.html</anchorfile>
      <anchor>ac9845342c0e54424f07f65a9a0900308</anchor>
      <arglist>(std::string locale)</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>key</name>
      <anchorfile>classreproweb_1_1I18N.html</anchorfile>
      <anchor>a96ac5582d833362eb97cf549e5c178fb</anchor>
      <arglist>(std::string locale, const std::string &amp;k)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>render</name>
      <anchorfile>classreproweb_1_1I18N.html</anchorfile>
      <anchor>abf9cfa2c0b85fdb4ba5fb32bfd6b28bb</anchor>
      <arglist>(std::string locale, const std::string &amp;txt)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::i18n_props</name>
    <filename>classreproweb_1_1i18n__props.html</filename>
  </compound>
  <compound kind="class">
    <name>reproweb::I18nMustacheView</name>
    <filename>classreproweb_1_1I18nMustacheView.html</filename>
    <base>reproweb::ViewMustacheDecorator</base>
  </compound>
  <compound kind="class">
    <name>reproweb::I18nSSIMustacheView</name>
    <filename>classreproweb_1_1I18nSSIMustacheView.html</filename>
    <base>reproweb::ViewMustacheDecorator</base>
  </compound>
  <compound kind="class">
    <name>reproweb::JWT</name>
    <filename>classreproweb_1_1JWT.html</filename>
    <member kind="function">
      <type></type>
      <name>JWT</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>acca10b82726c1933e8ec365fd809de2e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>JWT</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>afcfc852051bf90278a6f1e72c8cf6ad3</anchor>
      <arglist>(const std::string &amp;token)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>JWT</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>a0a5cc46363b92cd08795928854851afc</anchor>
      <arglist>(const Json::Value &amp;c)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>JWT</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>a9f8369fad7d58de3e7559951480eef1c</anchor>
      <arglist>(const Json::Value &amp;h, const Json::Value &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>parse</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>a91465e4d8a5057b69d95c325e9904f85</anchor>
      <arglist>(const std::string &amp;token)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>sign</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>aea60c6c34fc5c2e3ead92956b7d223f9</anchor>
      <arglist>(const std::string &amp;secret, size_t expires_s)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>verify</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>ac545156b350aed9f05ec6e23854d1baa</anchor>
      <arglist>(const std::string &amp;secret)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>sign</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>aeac78bad4a17a20706c07a88da672b53</anchor>
      <arglist>(cryptoneat::EVP_PKEY *privkey, size_t expires_s)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>verify</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>a86f81f7d7e96fb9c36d578f24ea238e8</anchor>
      <arglist>(cryptoneat::EVP_PKEY *pubkey)</arglist>
    </member>
    <member kind="function">
      <type>JWT &amp;</type>
      <name>header</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>ae67c9bd5a996e0112b27999640885b31</anchor>
      <arglist>(const Json::Value &amp;h)</arglist>
    </member>
    <member kind="function">
      <type>JWT &amp;</type>
      <name>claim</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>a478d9eb72dc8df26dcd512a5edce3613</anchor>
      <arglist>(const Json::Value &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>Json::Value</type>
      <name>header</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>a943ccdaea34cefd6e2ddcc27737eb4f0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Json::Value</type>
      <name>claim</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>aafff482a343a0248c264fefc88979185</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>signature</name>
      <anchorfile>classreproweb_1_1JWT.html</anchorfile>
      <anchor>aba66afe61d8e5249f012adff49cdc0e2</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::MemorySessionProvider</name>
    <filename>classreproweb_1_1MemorySessionProvider.html</filename>
    <base>reproweb::SessionProvider</base>
    <member kind="function">
      <type></type>
      <name>MemorySessionProvider</name>
      <anchorfile>classreproweb_1_1MemorySessionProvider.html</anchorfile>
      <anchor>a2d9f7972a80fdb1bb0137ff16a424fca</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MemorySessionProvider</name>
      <anchorfile>classreproweb_1_1MemorySessionProvider.html</anchorfile>
      <anchor>ab7bb585c55289baee97d2f9b1302b3fa</anchor>
      <arglist>(int timeout)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future&lt; Session &gt;</type>
      <name>get_session</name>
      <anchorfile>classreproweb_1_1MemorySessionProvider.html</anchorfile>
      <anchor>a90ba17e85f8f336cc264f8c77651829a</anchor>
      <arglist>(std::string sid)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future</type>
      <name>set_session</name>
      <anchorfile>classreproweb_1_1MemorySessionProvider.html</anchorfile>
      <anchor>a0554ff811650782f3ea6b91562b2b1da</anchor>
      <arglist>(Session session)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future</type>
      <name>remove_user_session</name>
      <anchorfile>classreproweb_1_1MemorySessionProvider.html</anchorfile>
      <anchor>a079627804293ae780812feee23f13e1a</anchor>
      <arglist>(Session session)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::miconv</name>
    <filename>classreproweb_1_1miconv.html</filename>
    <member kind="function">
      <type></type>
      <name>miconv</name>
      <anchorfile>classreproweb_1_1miconv.html</anchorfile>
      <anchor>a0d29e6d2a48d9cac187ab9e40ac50477</anchor>
      <arglist>(const char *from, const char *to)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>convert</name>
      <anchorfile>classreproweb_1_1miconv.html</anchorfile>
      <anchor>a55a4bbd10fc8fab056fe87e2a93382cc</anchor>
      <arglist>(const std::string &amp;input)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::vector&lt; std::string &gt;</type>
      <name>encodings</name>
      <anchorfile>classreproweb_1_1miconv.html</anchorfile>
      <anchor>a5d27ee534f0397b83a34ccec1a859e06</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::mustache</name>
    <filename>classreproweb_1_1mustache.html</filename>
    <member kind="function">
      <type></type>
      <name>mustache</name>
      <anchorfile>classreproweb_1_1mustache.html</anchorfile>
      <anchor>a250bba41ca261ba678f824c9d5d7d9a2</anchor>
      <arglist>(const std::string &amp;tpl)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>render</name>
      <anchorfile>classreproweb_1_1mustache.html</anchorfile>
      <anchor>ad318093a3a2d0c8fee5d6e53d9666147</anchor>
      <arglist>(Json::Value data)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::string</type>
      <name>render</name>
      <anchorfile>classreproweb_1_1mustache.html</anchorfile>
      <anchor>a7433c7fc1f853b07490adadcb478080d</anchor>
      <arglist>(const std::string &amp;tpl, Json::Value data)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::MustacheView</name>
    <filename>classreproweb_1_1MustacheView.html</filename>
    <base>reproweb::ViewMustacheDecorator</base>
  </compound>
  <compound kind="class">
    <name>reproweb::RedisSessionProvider</name>
    <filename>classreproweb_1_1RedisSessionProvider.html</filename>
    <base>reproweb::SessionProvider</base>
    <member kind="function">
      <type></type>
      <name>RedisSessionProvider</name>
      <anchorfile>classreproweb_1_1RedisSessionProvider.html</anchorfile>
      <anchor>a478ae1c96db5d121fa68a796017fabb0</anchor>
      <arglist>(std::shared_ptr&lt; reproredis::RedisPool &gt; r)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future&lt; Session &gt;</type>
      <name>get_session</name>
      <anchorfile>classreproweb_1_1RedisSessionProvider.html</anchorfile>
      <anchor>a5c49373c7709faa4fc2778b5e8d87e22</anchor>
      <arglist>(std::string sid)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future</type>
      <name>set_session</name>
      <anchorfile>classreproweb_1_1RedisSessionProvider.html</anchorfile>
      <anchor>a0ee677adbfaac699716bdbc5ef2707c9</anchor>
      <arglist>(Session session)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future</type>
      <name>remove_user_session</name>
      <anchorfile>classreproweb_1_1RedisSessionProvider.html</anchorfile>
      <anchor>a569986228a90e0b26e14a16c674cadf7</anchor>
      <arglist>(Session session)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::Session</name>
    <filename>classreproweb_1_1Session.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>valid</name>
      <anchorfile>classreproweb_1_1Session.html</anchorfile>
      <anchor>a8662cf5d0daa18b54c1c3f88c26f1d04</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>authenticated</name>
      <anchorfile>classreproweb_1_1Session.html</anchorfile>
      <anchor>a542bd7f173e8b7792e71463c699c9892</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>sid</name>
      <anchorfile>classreproweb_1_1Session.html</anchorfile>
      <anchor>a039f7a0cc62b07949564a5f95a2df223</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Json::Value</type>
      <name>data</name>
      <anchorfile>classreproweb_1_1Session.html</anchorfile>
      <anchor>a547c7b5ffb505e4240fa9b6e8efe2732</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>reproweb::session_filter</name>
    <filename>structreproweb_1_1session__filter.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>session_filter</name>
      <anchorfile>structreproweb_1_1session__filter.html</anchorfile>
      <anchor>a86e2fe0de4c2fd55f1231f81f6fb77a0</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, int prio=0)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::SessionFilter</name>
    <filename>classreproweb_1_1SessionFilter.html</filename>
  </compound>
  <compound kind="class">
    <name>reproweb::SessionProvider</name>
    <filename>classreproweb_1_1SessionProvider.html</filename>
    <member kind="function" virtualness="pure">
      <type>virtual repro::Future&lt; Session &gt;</type>
      <name>get_session</name>
      <anchorfile>classreproweb_1_1SessionProvider.html</anchorfile>
      <anchor>a1812d4967315a4c22d5058517fbc7fdb</anchor>
      <arglist>(std::string sid)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual repro::Future</type>
      <name>set_session</name>
      <anchorfile>classreproweb_1_1SessionProvider.html</anchorfile>
      <anchor>abfb7084b49d952616f03747af5f60633</anchor>
      <arglist>(Session session)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual repro::Future</type>
      <name>remove_user_session</name>
      <anchorfile>classreproweb_1_1SessionProvider.html</anchorfile>
      <anchor>a4d1c2b33d6a053323caff07696af4355</anchor>
      <arglist>(Session session)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::sse_controller</name>
    <filename>classreproweb_1_1sse__controller.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>sse_controller</name>
      <anchorfile>classreproweb_1_1sse__controller.html</anchorfile>
      <anchor>a42a8d84398b01ac010e4f7e1d126b728</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::SSEConnection</name>
    <filename>classreproweb_1_1SSEConnection.html</filename>
    <member kind="function">
      <type>void</type>
      <name>send</name>
      <anchorfile>classreproweb_1_1SSEConnection.html</anchorfile>
      <anchor>afb6ea3584f4374958d2b676295f41aca</anchor>
      <arglist>(const std::string &amp;s)</arglist>
    </member>
    <member kind="function">
      <type>SSEConnection *</type>
      <name>onClose</name>
      <anchorfile>classreproweb_1_1SSEConnection.html</anchorfile>
      <anchor>a3434f74b2a6f0ca330be0706d8074a16</anchor>
      <arglist>(T t)</arglist>
    </member>
    <member kind="function">
      <type>SSEConnection *</type>
      <name>onConnect</name>
      <anchorfile>classreproweb_1_1SSEConnection.html</anchorfile>
      <anchor>a45446c4adf7c1f6648b3102800172871</anchor>
      <arglist>(T t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>close</name>
      <anchorfile>classreproweb_1_1SSEConnection.html</anchorfile>
      <anchor>a973d1ea5c41551a09e6cd330846bf5ea</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::ssi_content</name>
    <filename>classreproweb_1_1ssi__content.html</filename>
  </compound>
  <compound kind="class">
    <name>reproweb::SSIMustacheView</name>
    <filename>classreproweb_1_1SSIMustacheView.html</filename>
    <base>reproweb::ViewMustacheDecorator</base>
  </compound>
  <compound kind="class">
    <name>reproweb::SSIResolver</name>
    <filename>classreproweb_1_1SSIResolver.html</filename>
    <member kind="function" static="yes">
      <type>static repro::Future&lt; std::string &gt;</type>
      <name>resolve</name>
      <anchorfile>classreproweb_1_1SSIResolver.html</anchorfile>
      <anchor>a6ecc1507776732bf1679cf1dd82f8fc1</anchor>
      <arglist>(prio::Request &amp;req, const std::string &amp;tmpl)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::string</type>
      <name>tmpl</name>
      <anchorfile>classreproweb_1_1SSIResolver.html</anchorfile>
      <anchor>a3183769be36841dea8e2d16fe2dcec95</anchor>
      <arglist>(prio::Request &amp;req, const std::string &amp;htdocs)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::static_content</name>
    <filename>classreproweb_1_1static__content.html</filename>
  </compound>
  <compound kind="struct">
    <name>reproweb::subscription</name>
    <filename>structreproweb_1_1subscription.html</filename>
    <member kind="function">
      <type></type>
      <name>subscription</name>
      <anchorfile>structreproweb_1_1subscription.html</anchorfile>
      <anchor>a5190f9b20016d3fd67e8d2310b7413fc</anchor>
      <arglist>(std::function&lt; void(Json::Value)&gt; f)</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(Json::Value)&gt;</type>
      <name>fun</name>
      <anchorfile>structreproweb_1_1subscription.html</anchorfile>
      <anchor>a07ff107985658f7ac69fdd72486f8404</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::TemplateView</name>
    <filename>classreproweb_1_1TemplateView.html</filename>
    <base>reproweb::AbstractView</base>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future&lt; std::string &gt;</type>
      <name>render_content</name>
      <anchorfile>classreproweb_1_1TemplateView.html</anchorfile>
      <anchor>a5cb525b63533d7f93afedac724eafa4f</anchor>
      <arglist>(prio::Request &amp;req, const std::string &amp;page, Json::Value value)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::TplStore</name>
    <filename>classreproweb_1_1TplStore.html</filename>
    <member kind="function">
      <type></type>
      <name>TplStore</name>
      <anchorfile>classreproweb_1_1TplStore.html</anchorfile>
      <anchor>ab8ccf445304d47112e836e6de679033e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>register_tpl</name>
      <anchorfile>classreproweb_1_1TplStore.html</anchorfile>
      <anchor>abe93089d493c6547525a5062173ad885</anchor>
      <arglist>(const std::string &amp;name, const std::string &amp;tpl)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>unregister_tpl</name>
      <anchorfile>classreproweb_1_1TplStore.html</anchorfile>
      <anchor>afe4da292c6cb372ddb13d1b42bbc8c37</anchor>
      <arglist>(const std::string &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>std::string &amp;</type>
      <name>get</name>
      <anchorfile>classreproweb_1_1TplStore.html</anchorfile>
      <anchor>a216e1ba5411195c030651f603ae2c355</anchor>
      <arglist>(const std::string &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>classreproweb_1_1TplStore.html</anchorfile>
      <anchor>a18661b6ffe251bd5c78f4ba4bd44410b</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>render</name>
      <anchorfile>classreproweb_1_1TplStore.html</anchorfile>
      <anchor>a806911fe4ef150429dcdcaead0606937</anchor>
      <arglist>(const std::string &amp;tpl, Json::Value val)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::validator</name>
    <filename>classreproweb_1_1validator.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>validator</name>
      <anchorfile>classreproweb_1_1validator.html</anchorfile>
      <anchor>a30440393e2604668132ec5cd4084ad79</anchor>
      <arglist>(const std::regex &amp;r)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getString</name>
      <anchorfile>classreproweb_1_1validator.html</anchorfile>
      <anchor>a3338a1474e34e04f9f5467f3db61c6bb</anchor>
      <arglist>(const std::string &amp;tainted, const std::string &amp;msg)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getInteger</name>
      <anchorfile>classreproweb_1_1validator.html</anchorfile>
      <anchor>a7594f5e6199760d935950573dc36af62</anchor>
      <arglist>(const std::string &amp;tainted, const std::string &amp;msg)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getDouble</name>
      <anchorfile>classreproweb_1_1validator.html</anchorfile>
      <anchor>a1d1f11626753f15478f9ea862a12820f</anchor>
      <arglist>(const std::string &amp;tainted, const std::string &amp;msg)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::view_templates</name>
    <filename>classreproweb_1_1view__templates.html</filename>
    <member kind="function">
      <type></type>
      <name>view_templates</name>
      <anchorfile>classreproweb_1_1view__templates.html</anchorfile>
      <anchor>ab232a11f0bc3db445172111655a5124a</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::ViewI18nDecorator</name>
    <filename>classreproweb_1_1ViewI18nDecorator.html</filename>
    <base>reproweb::AbstractView</base>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future&lt; std::string &gt;</type>
      <name>render_content</name>
      <anchorfile>classreproweb_1_1ViewI18nDecorator.html</anchorfile>
      <anchor>a8c2c345cbf7ade166d5a27f6aa6338b5</anchor>
      <arglist>(prio::Request &amp;req, const std::string &amp;page, Json::Value value)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::ViewMustacheDecorator</name>
    <filename>classreproweb_1_1ViewMustacheDecorator.html</filename>
    <base>reproweb::AbstractView</base>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future&lt; std::string &gt;</type>
      <name>render_content</name>
      <anchorfile>classreproweb_1_1ViewMustacheDecorator.html</anchorfile>
      <anchor>ac04d6c3afc23ad6b8b1964baf43ee858</anchor>
      <arglist>(prio::Request &amp;req, const std::string &amp;page, Json::Value value)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::ViewSSIDecorator</name>
    <filename>classreproweb_1_1ViewSSIDecorator.html</filename>
    <base>reproweb::AbstractView</base>
    <member kind="function" virtualness="virtual">
      <type>virtual repro::Future&lt; std::string &gt;</type>
      <name>render_content</name>
      <anchorfile>classreproweb_1_1ViewSSIDecorator.html</anchorfile>
      <anchor>a31e76abbbdf15393252f10fa77f6b203</anchor>
      <arglist>(prio::Request &amp;req, const std::string &amp;page, Json::Value value)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::WebApplicationContext</name>
    <filename>classreproweb_1_1WebApplicationContext.html</filename>
  </compound>
  <compound kind="class">
    <name>reproweb::WebServer</name>
    <filename>classreproweb_1_1WebServer.html</filename>
    <member kind="function">
      <type></type>
      <name>WebServer</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a0f6d018f8bacb0cf60178619d67c2949</anchor>
      <arglist>(diy::Context &amp;ctx)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>listen</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a61ecde4d7e2f42dfb6083d8ae9739614</anchor>
      <arglist>(int port)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>listen</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a868282ae4ca8cbf0903df93be1372e87</anchor>
      <arglist>(prio::SslCtx &amp;ssl, int port)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>listen</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a6c53edb0ad7f33a97117d62d773816b4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>shutdown</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a570f9907c4eb28321834f9ccee1c31cf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>configure</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>ad2b6c71b29261eef91ec753b015b82d1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>session</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a9949b114d367b8d49469ab088fd1527c</anchor>
      <arglist>(const std::string &amp;methods, const std::string &amp;paths, int prio)</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>session</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a2b793ca35bd066fac9ab03876791d9ea</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>filter</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a37a5eb99de1d0594b234f408e9618ea4</anchor>
      <arglist>(const std::string &amp;methods, const std::string &amp;paths, F f, int prio)</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>filter</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a344f7c43428f539acbaef54ab39434ca</anchor>
      <arglist>(const std::string &amp;name, F f)</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>views</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>ac1f70859e3d2cdb7d8c156320b9f2956</anchor>
      <arglist>(const std::string &amp;methods)</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>htdocs</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a7586fcadb695f71bbeac7869946b7311</anchor>
      <arglist>(const std::string &amp;methods)</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>htdocs</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a99816296926b7c18a16c06624a37d844</anchor>
      <arglist>(const std::string &amp;methods, const std::string &amp;mime)</arglist>
    </member>
    <member kind="function">
      <type>WebServer &amp;</type>
      <name>i18n</name>
      <anchorfile>classreproweb_1_1WebServer.html</anchorfile>
      <anchor>a56be3d41e464f061c4c459fcde6620c3</anchor>
      <arglist>(const std::string &amp;path, const std::vector&lt; std::string &gt; &amp;locales)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::ws_controller</name>
    <filename>classreproweb_1_1ws__controller.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>ws_controller</name>
      <anchorfile>classreproweb_1_1ws__controller.html</anchorfile>
      <anchor>ac8b19fb8cb63a8848ded91eb33882d76</anchor>
      <arglist>(const std::string &amp;path)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>reproweb::WsConnection</name>
    <filename>classreproweb_1_1WsConnection.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>send</name>
      <anchorfile>classreproweb_1_1WsConnection.html</anchorfile>
      <anchor>a918ff567d442b6a7d32830abbce8dca2</anchor>
      <arglist>(int opcode, const std::string &amp;s)</arglist>
    </member>
    <member kind="function">
      <type>WsConnection *</type>
      <name>onMsg</name>
      <anchorfile>classreproweb_1_1WsConnection.html</anchorfile>
      <anchor>a66219d04398dfba40d7571aee650c3fc</anchor>
      <arglist>(T t)</arglist>
    </member>
    <member kind="function">
      <type>WsConnection *</type>
      <name>onClose</name>
      <anchorfile>classreproweb_1_1WsConnection.html</anchorfile>
      <anchor>aceb407da1f54823dfbcc7d73182a30a2</anchor>
      <arglist>(T t)</arglist>
    </member>
    <member kind="function">
      <type>WsConnection *</type>
      <name>onConnect</name>
      <anchorfile>classreproweb_1_1WsConnection.html</anchorfile>
      <anchor>a9af9dd864c994102f229be52ad3d2184</anchor>
      <arglist>(T t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>close</name>
      <anchorfile>classreproweb_1_1WsConnection.html</anchorfile>
      <anchor>adb15131290ff6c0ba9248c4e6489758d</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>controller</name>
    <title>Controller</title>
    <filename>group__controller.html</filename>
    <member kind="typedef">
      <type>std::function&lt; void(prio::Request &amp;req, prio::Response &amp;res)&gt;</type>
      <name>http_handler_t</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga2cb9980c09579508bb89e1330b613c15</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>std::function&lt; void(prio::Request &amp;req, prio::Response &amp;res, std::shared_ptr&lt; FilterChain &gt; chain)&gt;</type>
      <name>http_filter_t</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gae47f9bc633eaf551c9f65633e8cf98f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>router&lt; F &gt;</type>
      <name>route</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gaac161bdfc52ed1ce5e7d22bd043501db</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F fun)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>GET</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gaef23d8afb7170f8d42d798dbab327d06</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>POST</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gaac05d4e569bbf3f832bfb13aa00cf4a9</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>DEL</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gab9144873d8e9fdbc3d1f059a07c6b22e</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>PUT</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga4b01197386573aa8d48c752ba034ae54</anchor>
      <arglist>(const std::string &amp;p, F f)</arglist>
    </member>
    <member kind="function">
      <type>filter_router&lt; F &gt;</type>
      <name>filter</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gabc3c899d9b551e6292b99f1fa36a963f</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F f, int priority=0)</arglist>
    </member>
    <member kind="function">
      <type>completion_filter_router&lt; F &gt;</type>
      <name>completion_filter</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>gabbad223182df93ef4358941ef9349f11</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F f, int priority=0)</arglist>
    </member>
    <member kind="function">
      <type>flush_filter_router&lt; F &gt;</type>
      <name>flush_filter</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga5415e1fdb035f3945730593e337192b3</anchor>
      <arglist>(const std::string &amp;m, const std::string &amp;p, F f, int priority=0)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>render_error</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga19666b4f4490393dcd8057b86233a2a9</anchor>
      <arglist>(prio::Response &amp;res)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>redirect</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga6dbba79ffb7be53f096189a323e0beca</anchor>
      <arglist>(prio::Response &amp;res, Args ... args)</arglist>
    </member>
    <member kind="function">
      <type>exception_handler&lt; F &gt;</type>
      <name>ex_handler</name>
      <anchorfile>group__controller.html</anchorfile>
      <anchor>ga2caa64e5bccb8fa70b6e70db590688ee</anchor>
      <arglist>(F fun)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>session</name>
    <title>Session</title>
    <filename>group__session.html</filename>
    <class kind="class">reproweb::RedisSessionProvider</class>
    <class kind="class">reproweb::Session</class>
    <class kind="class">reproweb::SessionProvider</class>
    <class kind="class">reproweb::MemorySessionProvider</class>
  </compound>
  <compound kind="group">
    <name>view</name>
    <title>View</title>
    <filename>group__view.html</filename>
    <class kind="class">reproweb::SSIResolver</class>
    <class kind="class">reproweb::ssi_content</class>
    <class kind="class">reproweb::mustache</class>
    <class kind="class">reproweb::TplStore</class>
    <class kind="class">reproweb::view_templates</class>
    <class kind="class">reproweb::AbstractView</class>
    <class kind="class">reproweb::TemplateView</class>
    <class kind="class">reproweb::ViewI18nDecorator</class>
    <class kind="class">reproweb::ViewMustacheDecorator</class>
    <class kind="class">reproweb::ViewSSIDecorator</class>
    <class kind="class">reproweb::I18nSSIMustacheView</class>
    <class kind="class">reproweb::I18nMustacheView</class>
    <class kind="class">reproweb::MustacheView</class>
    <class kind="class">reproweb::SSIMustacheView</class>
  </compound>
  <compound kind="group">
    <name>json</name>
    <title>Json</title>
    <filename>group__json.html</filename>
    <class kind="struct">reproweb::subscription</class>
    <class kind="class">reproweb::EventBus</class>
    <class kind="class">reproweb::JWT</class>
    <member kind="function">
      <type>repro::Future</type>
      <name>forEach</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga09241712ea12f24c4ed37b03a2b7cdf4</anchor>
      <arglist>(Json::Value json, F f)</arglist>
    </member>
    <member kind="function">
      <type>Json::Value</type>
      <name>sort</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga13a4de05719b7d00a37a39878ebe9af3</anchor>
      <arglist>(const Json::Value &amp;arrayIn, const std::string member)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>json_response</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga0668dd651539527c033e05e645473a14</anchor>
      <arglist>(prio::Response &amp;res, Json::Value json)</arglist>
    </member>
    <member kind="function">
      <type>auto</type>
      <name>json_err</name>
      <anchorfile>group__json.html</anchorfile>
      <anchor>ga50d38a00c8791085c9dd1224394873b6</anchor>
      <arglist>(prio::Response &amp;res)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>webserver</name>
    <title>Webserver</title>
    <filename>group__webserver.html</filename>
    <class kind="class">reproweb::WebApplicationContext</class>
    <class kind="class">reproweb::Config</class>
    <class kind="class">reproweb::WebServer</class>
  </compound>
  <compound kind="group">
    <name>ws</name>
    <title>Ws</title>
    <filename>group__ws.html</filename>
    <class kind="class">reproweb::SSEConnection</class>
    <class kind="class">reproweb::sse_controller</class>
    <class kind="class">reproweb::WsConnection</class>
    <class kind="class">reproweb::ws_controller</class>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Modern C++ for web development. With Promises. And Coroutines.</title>
    <filename>index</filename>
  </compound>
</tagfile>
