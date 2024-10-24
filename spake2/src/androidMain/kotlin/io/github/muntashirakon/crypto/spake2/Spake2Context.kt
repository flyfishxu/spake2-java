/*
 * Copyright (C) 2021 Muntashir Al-Islam
 *
 * Licensed according to the LICENSE file in this repository.
 */
package io.github.muntashirakon.crypto.spake2

import java.lang.IllegalStateException
import java.lang.UnsupportedOperationException
import javax.security.auth.Destroyable

/**
 * Maximum message size in bytes
 */
private const val MAX_MSG_SIZE: Int = 32

/**
 * Maximum key size in bytes
 */
private const val MAX_KEY_SIZE: Int = 64

@Suppress("unused")
class Spake2Context(
    myRole: Spake2Role, myName: ByteArray?, theirName: ByteArray?
) : Destroyable {
    private external fun generateMessage(ctx: Long, password: ByteArray?): ByteArray
    private external fun processMessage(ctx: Long, theirMessage: ByteArray?): ByteArray
    private external fun destroy(ctx: Long)
    private external fun allocNewContext(
        myRole: Int, myName: ByteArray?, theirName: ByteArray?
    ): Long

    init {
        System.loadLibrary("spake2")
    }

    private var mIsDestroyed = false
    private val mCtx: Long = allocNewContext(myRole.ordinal, myName, theirName).also {
        if (it == 0L) {
            throw UnsupportedOperationException("Could not allocate native context")
        }
    }

    @Throws(IllegalStateException::class)
    fun generateMessage(password: ByteArray?): ByteArray {
        check(!mIsDestroyed) { "The context was destroyed." }
        val myMsg = generateMessage(mCtx, password)
        checkNotNull(myMsg) { "Generated empty message" }
        System.arraycopy(myMsg, 0, ByteArray(MAX_MSG_SIZE), 0, MAX_MSG_SIZE)
        return myMsg
    }

    @Throws(IllegalStateException::class)
    fun processMessage(theirMessage: ByteArray?): ByteArray {
        check(!mIsDestroyed) { "The context was destroyed." }
        val key = processMessage(mCtx, theirMessage)
        checkNotNull(key) { "No key was returned" }
        return key
    }

    override fun isDestroyed(): Boolean {
        return mIsDestroyed
    }

    override fun destroy() {
        mIsDestroyed = true
        destroy(mCtx)
    }
}