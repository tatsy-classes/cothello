import othello


def test_repr():
    env = othello.make()
    assert len(env.legal_actions()) == 0

    env.reset()
    actions = env.legal_actions()
    assert str(actions[0]) == "BLACK 5F"
