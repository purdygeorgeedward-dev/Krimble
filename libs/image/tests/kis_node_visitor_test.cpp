/*
 *  SPDX-FileCopyrightText: 2007 Boudewijn Rempt boud @valdyas.org
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "kis_node_visitor_test.h"

#include <simpletest.h>
#include "kis_node_visitor.h"

class TestNodeVisitor : public KisNodeVisitor
{
public:

    TestNodeVisitor() {}

    ~TestNodeVisitor() override {}

    bool visit(KisNode * ) override {
        return true;
    }

    bool visit(KisPaintLayer * ) override {
        return true;
    }

    bool visit(KisGroupLayer * ) override {
        return true;
    }

    bool visit(KisAdjustmentLayer * ) override {
        return true;
    }

    bool visit(KisExternalLayer * ) override {
        return true;
    }

    bool visit(KisGeneratorLayer * ) override {
        return true;
    }

    bool visit(KisCloneLayer * ) override {
        return true;
    }

    bool visit(KisFilterMask * ) override {
        return true;
    }

    bool visit(KisTransformMask * ) override {
        return true;
    }

    bool visit(KisTransparencyMask * ) override {
        return true;
    }

    bool visit(KisSelectionMask * ) override {
        return true;
    }

    bool visit(KisColorizeMask * ) override {
        return true;
    }

};

void KisNodeVisitorTest::testCreation()
{
    TestNodeVisitor v;
}

void KisNodeVisitorTest::testFullImage()
{
}

SIMPLE_TEST_MAIN(KisNodeVisitorTest)
