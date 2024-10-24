plugins {
    `maven-publish`
    alias(libs.plugins.kotlinMultiplatform)
    alias(libs.plugins.androidLibrary)
}

kotlin {
    jvm {
        kotlin {
            jvmToolchain(21)
        }
    }

    androidTarget {
        kotlin {
            jvmToolchain(21)
        }
        publishAllLibraryVariants()
    }

    sourceSets {
        val jvmTest by getting {
            dependencies {
                implementation(libs.kotlin.test)
            }
        }
    }
}


android {
    namespace = "io.github.muntashirakon.crypto"
    compileSdk = 35
    ndkVersion = "28.0.12433566"

    externalNativeBuild {
        cmake {
            path = file("src/androidMain/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    defaultConfig {
        minSdk = 23
        externalNativeBuild {
            cmake {
                arguments += listOf("-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON")
            }
        }
    }

}

allprojects {
    group = "io.github.muntashirakon.crypto"
    version = "0.0.5"
}

tasks.withType<Test> {
    enabled = false
}
