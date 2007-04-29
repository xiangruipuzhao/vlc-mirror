/*****************************************************************************
 * simple_preferences.cpp : "Simple preferences"
 ****************************************************************************
 * Copyright (C) 2006-2007 the VideoLAN team
 * $Id: preferences.cpp 16348 2006-08-25 21:10:10Z zorglub $
 *
 * Authors: Clément Stenac <zorglub@videolan.org>
 *          Antoine Cellerier <dionoea@videolan.org>
 *          Jean-Baptiste Kempf <jb@videolan.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#include <QString>
#include <QFont>
#include <QToolButton>
#include <QButtonGroup>
#include <QUrl>

#include "components/simple_preferences.hpp"
#include "components/preferences_widgets.hpp"
#include "qt4.hpp"

#include <vlc_config_cat.h>

#include "ui/sprefs_audio.h"
#include "ui/sprefs_input.h"
#include "ui/sprefs_video.h"
#include "ui/sprefs_subtitles.h"
#include "ui/sprefs_hotkeys.h"
#include "ui/sprefs_interface.h"

#define ICON_HEIGHT 64
#define BUTTON_HEIGHT 74

/*********************************************************************
 * The List of categories
 *********************************************************************/
SPrefsCatList::SPrefsCatList( intf_thread_t *_p_intf, QWidget *_parent ) :
                                  QWidget( _parent ), p_intf( _p_intf )
{
    QVBoxLayout *layout = new QVBoxLayout();

    QButtonGroup *buttonGroup = new QButtonGroup( this );
    buttonGroup->setExclusive ( true );
    CONNECT( buttonGroup, buttonClicked ( int ),
            this, switchPanel( int ) );

#define ADD_CATEGORY( button, label, icon, numb )                           \
    QToolButton * button = new QToolButton( this );                         \
    button->setIcon( QIcon( ":/pixmaps/" #icon ) );                         \
    button->setIconSize( QSize( ICON_HEIGHT , ICON_HEIGHT ) );              \
    button->setText( label );                                               \
    button->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );              \
    button->resize( BUTTON_HEIGHT , BUTTON_HEIGHT);                         \
    button->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding) ;  \
    button->setAutoRaise( true );                                           \
    button->setCheckable( true );                                           \
    buttonGroup->addButton( button, numb );                                 \
    layout->addWidget( button );

    ADD_CATEGORY( SPrefsInterface, qtr("Interface"),
                  spref_cone_Interface_64.png, 0 );
    ADD_CATEGORY( SPrefsAudio, qtr("Audio"), spref_cone_Audio_64.png, 1 );
    ADD_CATEGORY( SPrefsVideo, qtr("Video"), spref_cone_Video_64.png, 2 );
    ADD_CATEGORY( SPrefsSubtitles, qtr("Subtitles"),
                  spref_cone_Subtitles_64.png, 3 );
    ADD_CATEGORY( SPrefsInputAndCodecs, qtr("Input and Codecs"),
                  spref_cone_Input_64.png, 4 );
    ADD_CATEGORY( SPrefsHotkeys, qtr("Hotkeys"), spref_cone_Hotkeys_64.png, 5 );

    SPrefsInterface->setChecked( true );
    layout->setMargin( 0 );
    layout->setSpacing( 1 );

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setLayout( layout );

}

void SPrefsCatList::switchPanel( int i )
{
    emit currentItemChanged( i );
}

/*********************************************************************
 * The Panels
 *********************************************************************/
SPrefsPanel::SPrefsPanel( intf_thread_t *_p_intf, QWidget *_parent,
                          int number ) : QWidget( _parent ), p_intf( _p_intf )
{
    module_config_t *p_config;
    ConfigControl *control;

#define CONFIG_GENERIC( option, type, label, qcontrol )                   \
            p_config =  config_FindConfig( VLC_OBJECT(p_intf), option );  \
            if( p_config )                                                \
            {                                                             \
                control =  new type ## ConfigControl( VLC_OBJECT(p_intf), \
                           p_config, label, ui.qcontrol, false );         \
                controls.append( control );                               \
            }

#define CONFIG_GENERIC_NO_BOOL( option, type, label, qcontrol )           \
            p_config =  config_FindConfig( VLC_OBJECT(p_intf), option );  \
            if( p_config )                                                \
            {                                                             \
                control =  new type ## ConfigControl( VLC_OBJECT(p_intf), \
                           p_config, label, ui.qcontrol );                \
                controls.append( control );                               \
            }

#define CONFIG_GENERIC_FILE( option, type, label, qcontrol, qbutton )         \
                p_config =  config_FindConfig( VLC_OBJECT(p_intf), option );  \
                if( p_config )                                                \
                {                                                             \
                    control =  new type ## ConfigControl( VLC_OBJECT(p_intf), \
                               p_config, label, ui.qcontrol, ui.qbutton,      \
                            false );                                          \
                    controls.append( control );                               \
                }

#define START_SPREFS_CAT( name , label )    \
        case SPrefs ## name:                \
        {                                   \
            Ui::SPrefs ## name ui;          \
            ui.setupUi( panel );            \
            panel_label->setText( qtr( label ) );

#define END_SPREFS_CAT      \
            break;          \
        }

    QVBoxLayout *panel_layout = new QVBoxLayout();
    QWidget *panel = new QWidget();
    panel_layout->setMargin( 3 );

    // Title Label
    QLabel *panel_label = new QLabel;
    QFont labelFont = QApplication::font( static_cast<QWidget*>(0) );
    labelFont.setPointSize( labelFont.pointSize() + 6 );
    labelFont.setFamily( "Verdana" );
    panel_label->setFont( labelFont );

    // Title <hr>
    QFrame *title_line = new QFrame;
    title_line->setFrameShape(QFrame::HLine);
    title_line->setFrameShadow(QFrame::Sunken);

    QFont italicFont = QApplication::font( static_cast<QWidget*>(0) );
    italicFont.setItalic( true );

    switch( number )
    {
        /* Video Panel Implementation */
        START_SPREFS_CAT( Video , "General video settings" );
           CONFIG_GENERIC( "video", Bool, NULL, enableVideo );

            CONFIG_GENERIC( "fullscreen", Bool, NULL, fullscreen );
            CONFIG_GENERIC( "overlay", Bool, NULL, overlay );
            CONFIG_GENERIC( "video-on-top", Bool, NULL, alwaysOnTop );
            CONFIG_GENERIC( "video-deco", Bool, NULL, windowDecorations );
            CONFIG_GENERIC( "skip-frames" , Bool, NULL, skipFrames);
            CONFIG_GENERIC( "vout", Module, NULL, outputModule );

#ifdef WIN32
            CONFIG_GENERIC( "directx-wallpaper" , Bool , NULL, wallpaperMode );
            CONFIG_GENERIC( "directx-device", StringList, NULL, 
                    dXdisplayDevice );
#else
            ui.directXBox->setVisible( false );
#endif

            CONFIG_GENERIC_FILE( "snapshot-path", Directory, NULL,
                    snapshotsDirectory, snapshotsDirectoryBrowse );
            CONFIG_GENERIC( "snapshot-prefix", String, NULL, snapshotsPrefix );
            CONFIG_GENERIC( "snapshot-sequential", Bool, NULL,
                            snapshotsSequentialNumbering );
            CONFIG_GENERIC( "snapshot-format", StringList, NULL,
                            snapshotsFormat );
         END_SPREFS_CAT;

         /* Audio Panel Implementation */
        START_SPREFS_CAT( Audio,  "General audio settings" );
#ifdef WIN32
            ui.OSSBrowse->hide();
            ui.OSSDevice->hide();
            ui.OSSLabel->hide();
            ui.alsaDevice->hide();
            ui.alsaLabel->hide();
#else
            ui.DirectXLabel->setVisible( false );
            ui.DirectXDevice->setVisible( false );
#endif
         CONFIG_GENERIC( "audio", Bool, NULL, enableAudio );

         CONFIG_GENERIC_NO_BOOL( "volume" ,  IntegerRangeSlider, NULL, 
                 defaultVolume );
         CONFIG_GENERIC( "audio-language" , StringList , NULL, 
                    preferredAudioLanguage ); //FIXME

         CONFIG_GENERIC( "spdif" , Bool , NULL, spdifBox );
         CONFIG_GENERIC( "force-dolby-surround" , IntegerList , NULL, 
                    detectionDolby );

         CONFIG_GENERIC( "aout" , Module , NULL, outputModule );
#ifndef WIN32
         CONFIG_GENERIC( "alsadev" , StringList , NULL, alsaDevice );
         CONFIG_GENERIC_FILE( "dspdev" , File , NULL, OSSDevice, OSSBrowse );
#else
         CONFIG_GENERIC( "directx-audio-device" , IntegerList, NULL, 
                 DirectXDevice );
#endif
         CONFIG_GENERIC_FILE( "audiofile-file" , File , NULL, FileName, 
                 fileBrowseButton );

         CONFIG_GENERIC( "headphone-dolby" , Bool , NULL, headphoneEffect );
//         CONFIG_GENERIC( "" , Bool, NULL, ); activation of normalizer //FIXME
         CONFIG_GENERIC_NO_BOOL( "norm-max-level" , Float , NULL, 
                 volNormalizer );
         CONFIG_GENERIC( "audio-visual" , Module , NULL, visualisation);
        END_SPREFS_CAT;

        /* Input and Codecs Panel Implementation */
        START_SPREFS_CAT( InputAndCodecs, "Input & Codecs settings"  );
          /* Disk Devices */
/*          CONFIG_GENERIC( );*/ //FIXME

          CONFIG_GENERIC_NO_BOOL( "server-port", Integer, NULL, UDPPort );
          CONFIG_GENERIC( "http-proxy", String , NULL, proxy );

          /* Caching */
/*          CONFIG_GENERIC( );*/ //FIXME

          CONFIG_GENERIC_NO_BOOL( "ffmpeg-pp-q", Integer, NULL, PostProcLevel );
          CONFIG_GENERIC( "avi-index", IntegerList, NULL, AviRepair );
          CONFIG_GENERIC( "rtsp-tcp", Bool, NULL, RTSP_TCPBox );

          CONFIG_GENERIC( "timeshift-force", Bool, NULL, timeshiftBox );
          CONFIG_GENERIC( "dump-force", Bool, NULL, DumpBox );
//        CONFIG_GENERIC( "", Bool, NULL, RecordBox ); //FIXME activate record 
        END_SPREFS_CAT;

        /* Interface Panel */
        START_SPREFS_CAT( Interface, "Interface settings" );
            ui.defaultLabel->setFont( italicFont );
            ui.skinsLabel->setFont( italicFont );

#if defined( WIN32 ) || defined (__APPLE__)
            CONFIG_GENERIC( "language", StringList, NULL, language );//FIXME
#else
            ui.language->hide();
            ui.languageLabel->hide();
#endif

           /* interface */
/*            p_config = config_FindConfig( VLC_OBJECT(p_intf), "intf" );
            if( p_config->value.psz && strcmp( p_config->value.psz, "qt4" ))
            {
                ui.qt4->setChecked( true );
                control =  new StringConfigControl( VLC_OBJECT(p_intf),
                                 p_config, NULL, ui.qt4 );
                controls.append( control );
            }
            if( p_config->value.psz && strcmp( p_config->value.psz, "skins2" ))
            {
                    ui.skins->setChecked( true );
            }*/
//            CONFIG_GENERIC( "intf", Module, NULL, Interface );
            //FIXME interface choice
            CONFIG_GENERIC( "qt-always-video", Bool, NULL, qtAlwaysVideo );
            CONFIG_GENERIC_FILE( "skins2-last", File, NULL, fileSkin, 
                    skinBrowse );
#if defined( WIN32 ) || defined(HAVE_DBUS_3)
            CONFIG_GENERIC( "one-instance", Bool, NULL, OneInterfaceMode );
            CONFIG_GENERIC( "playlist-enqueue", Bool, NULL, 
                    EnqueueOneInterfaceMode );
#else
            ui.OneInterfaceBox->hide();
#endif
        END_SPREFS_CAT;

        START_SPREFS_CAT( Subtitles, "Subtitles & OSD settings" );
            CONFIG_GENERIC( "osd", Bool, NULL, OSDBox);

            CONFIG_GENERIC( "subsdec-encoding", StringList, NULL, encoding );
            CONFIG_GENERIC( "sub-language", String, NULL, preferredLanguage );//FIXME
            CONFIG_GENERIC_FILE( "freetype-font", File, NULL, font, 
                    fontBrowse ); 
            CONFIG_GENERIC( "freetype-color", IntegerList, NULL, fontColor );
            CONFIG_GENERIC( "freetype-rel-fontsize", IntegerList, NULL,
                            fontSize );
            CONFIG_GENERIC( "freetype-effect", IntegerList, NULL, effect );

        END_SPREFS_CAT;

        START_SPREFS_CAT( Hotkeys, "Configure Hotkeys" );
        //FIMXE
        END_SPREFS_CAT;
        }

    panel_layout->addWidget(panel_label);
    panel_layout->addWidget(title_line);
    panel_layout->addWidget( panel );
    panel_layout->addStretch( 2 );

    this->setLayout(panel_layout);
}

void SPrefsPanel::apply()
{
    QList<ConfigControl *>::Iterator i;
    for( i = controls.begin() ; i != controls.end() ; i++ )
    {
        ConfigControl *c = qobject_cast<ConfigControl *>(*i);
        c->doApply( p_intf );
    }
}

void SPrefsPanel::clean()
{}

