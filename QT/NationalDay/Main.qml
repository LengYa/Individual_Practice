import QtQuick
import QtQuick.Window
import QtQuick.Particles
import QtQuick.Effects
import QtMultimedia

Window {
    width: 1280; height: 720
    visible: true
    title: qsTr("庆祝国庆76周年")
    color: "#0a0a1a"

    // 背景音乐
    MediaPlayer {
        id: bgMusic
        source: "qrc:/sound.wav"
        audioOutput: AudioOutput {}
        loops: MediaPlayer.Infinite
    }

    Component.onCompleted: bgMusic.play()

    // 背景
    Image {
        id: bg
        source: "qrc:/images/tiananmen.jpg"
        anchors.fill: parent
        opacity: 0.85
    }

    ParticleSystem {
        id: particleSys
        running: true
    }

    // 火箭粒子
    ImageParticle {
        system: particleSys
        groups: ["rocket"]
        source: "qrc:///particleresources/glowdot.png"
        color: "gold"
        colorVariation: 0.4
        entryEffect: ImageParticle.Fade
    }

    // 烟雾粒子
    ImageParticle {
        system: particleSys
        groups: ["smoke"]
        source: "qrc:///particleresources/star.png"
        color: "#80ffffff"
        colorVariation: 0.3
        alpha: 0.6
        rotation: 0
        rotationVariation: 180
        rotationVelocity: 45
        entryEffect: ImageParticle.Fade
    }

    // 爆炸粒子
    ImageParticle {
        system: particleSys
        groups: ["explode"]
        source: "qrc:///particleresources/star.png"
        color: Qt.hsla(Math.random(), 0.8, 0.6, 0.9)
        colorVariation: 0.8
        rotation: 0
        rotationVariation: 360
        rotationVelocity: 180
        entryEffect: ImageParticle.Scale
    }

    // 左边发射器
    Repeater {
        model: 5
        Emitter {
            id: leftRocketEmitter
            system: particleSys
            group: "rocket"

            x: index * 150 + 100
            y: parent.height - 50
            width: 8
            height: 8

            emitRate: 1
            maximumEmitted: 2
            lifeSpan: 3000
            lifeSpanVariation: 500
            size: 32
            sizeVariation: 8
            endSize: 16

            velocity: AngleDirection {
                angle: 270
                magnitude: 350
                magnitudeVariation: 80
            }

            acceleration: AngleDirection {
                angle: 90
                magnitude: 60
            }
        }
    }

    // 右边发射器
    Repeater {
        model: 5
        Emitter {
            id: rightRocketEmitter
            system: particleSys
            group: "rocket"

            x: parent.width - (index * 150 + 100)
            y: parent.height - 50
            width: 8
            height: 8

            emitRate: 1
            maximumEmitted: 2
            lifeSpan: 3000
            lifeSpanVariation: 500
            size: 32
            sizeVariation: 8
            endSize: 16

            velocity: AngleDirection {
                angle: 270
                magnitude: 350
                magnitudeVariation: 80
            }

            acceleration: AngleDirection {
                angle: 90
                magnitude: 60
            }
        }
    }

    // 烟雾轨迹
    TrailEmitter {
        id: smokeEmitter
        system: particleSys
        emitHeight: 2
        emitWidth: 6
        group: "smoke"
        follow: "rocket"
        emitRatePerParticle: 128
        lifeSpan: 800
        lifeSpanVariation: 200
        size: 20
        sizeVariation: 6
        endSize: 4

        velocity: AngleDirection {
            angle: 90
            angleVariation: 15
            magnitude: 80
        }
    }

    // 顶部摩擦效果
    Friction {
        groups: ["rocket"]
        anchors.top: parent.top
        width: parent.width
        height: parent.height / 2
        system: particleSys
        threshold: 5
        factor: 0.85
    }

    // 底部湍流效果
    Turbulence {
        groups: ["rocket"]
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height / 3
        system: particleSys
        strength: 35
    }

    // 到达顶部时触发爆炸
    GroupGoal {
        id: explosionTrigger
        anchors.top: parent.top
        width: parent.width
        height: parent.height / 6
        system: particleSys
        groups: ["rocket"]
        goalState: "explosion"
        jump: true
    }

    // 爆炸粒子组
    ParticleGroup {
        name: "explosion"
        system: particleSys

        TrailEmitter {
            id: explosionEmitter
            group: "explode"
            follow: "rocket"
            lifeSpan: 1500
            lifeSpanVariation: 300
            emitRatePerParticle: 300
            size: 36
            sizeVariation: 12
            endSize: 8

            velocity: AngleDirection {
                angle: -90
                angleVariation: 360
                magnitude: 80
                magnitudeVariation: 50
            }

            acceleration: AngleDirection {
                angle: 90
                magnitude: 40
            }
        }
    }

    //文字
    Column {
        anchors.top: parent.top
        anchors.topMargin: 30  // 添加一些上边距，使文字不会紧贴顶部

        width: parent.width
        //anchors.horizontalCenter: parent.horizontalCenter

        Text {
            text: qsTr("庆祝中华人民共和国成立76周年")
            anchors.horizontalCenter: parent.horizontalCenter
            font {
                family: "Microsoft YaHei"
                pixelSize: 48
                bold: true
            }
            color: "#FFD700"
        }
        Text {
            text: qsTr("祝祖国永远繁荣昌盛")
            anchors.horizontalCenter: parent.horizontalCenter
            font {
                family: "Microsoft YaHei"
                pixelSize: 32
            }
            color: "#FFFFFF"
        }

    }
}
